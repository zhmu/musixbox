#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpg123.h>
#include <math.h>
#include "decode_mp3_mpg123.h"
#include "exceptions.h"
#include "output.h"

using namespace std;

static bool libmpg123_initialized = false;

/*
 * This is a terrible, terrible kludge: libmpg123's API is based on fd's,
 * which are stored as int's (this is likely to handle flawless POSIX
 * integration) - however, if our read/seek is called, we need to know
 * which *object* is being used; and this doesn't necessarily have to be
 * storable in an integer...
 *
 * To prevent this, we have an array of decoders and the fd points to the
 * index in this array. The array will be resized if needed. Yes, this is
 * horrible, but unfixable with such a sucky API :-/
 */
static pthread_mutex_t libmpg123_mutex;
static DecoderMP3_MPG123** libmpg123_decoder = NULL;
static int libmpg123_numdecoders = 0;

/*
 * Note: we assume that the current player thread will be destroyed before
 * any my_mpg123_XXX() functions are called. This should be reasonable, as
 * we only add/remove players and don't mess with any others.
 */
ssize_t
my_mpg123_read(int fd, void* buf, size_t len)
{
	/* safeguard */
	if (fd >= libmpg123_numdecoders)
		throw DecoderException("impossible file descriptor!");

	DecoderMP3_MPG123* decoder = libmpg123_decoder[fd];
	return decoder->input->read((char*)buf, len);
}

off_t
my_mpg123_seek(int fd, off_t offset, int whence)
{
	/* safeguard */
	if (fd >= libmpg123_numdecoders)
		throw DecoderException("impossible file descriptor!");

	DecoderMP3_MPG123* decoder = libmpg123_decoder[fd];
	Input* input = decoder->input;
	off_t new_offset;
	switch (whence) {
		case SEEK_SET:
		default:
			new_offset = offset;
			break;
		case SEEK_CUR:
			new_offset = input->getCurrentPosition() + offset;
			break;
		case SEEK_END:
			new_offset = input->getLength() + offset;
			break;
	}
	return decoder->input->seek(new_offset) ? new_offset : -1;
}

DecoderMP3_MPG123::DecoderMP3_MPG123(Player* p, Input* i, Output* o, Visualizer* v)
	: Decoder(p, i, o, v)
{
	/*
	 * libmpg123 wants to be initialized just once; this seems the best place.
	 * This is also a good place to initialize our own administration.
	 */
	if (!libmpg123_initialized) {
		if (mpg123_init() != MPG123_OK)
			throw DecoderException("mpg123_init() failed");
		pthread_mutex_init(&libmpg123_mutex, NULL);
		libmpg123_initialized = true;
	}

	/*
	 * Find a slot in the decoder list; we need this to distinguish between
	 * multiple objects.
	 */
	pthread_mutex_lock(&libmpg123_mutex);
	unsigned int decoderidx;
	for (decoderidx = 0; decoderidx < libmpg123_numdecoders; decoderidx++)
		if (libmpg123_decoder[decoderidx] == NULL)
			break;

	if (libmpg123_numdecoders == decoderidx) {
		libmpg123_decoder = (DecoderMP3_MPG123**)realloc(libmpg123_decoder, ((libmpg123_numdecoders + 1) * 2) * sizeof(DecoderMP3_MPG123*));
		if (libmpg123_decoder == NULL)
			throw DecoderException("out of memory");
		libmpg123_numdecoders = (libmpg123_numdecoders + 1) * 2;
	}

	libmpg123_decoder[decoderidx] = this;
	pthread_mutex_unlock(&libmpg123_mutex);

	int errcode;
	pars = mpg123_new_pars(&errcode);
	if (errcode != MPG123_OK)
		throw DecoderException(string("mpg123_new_pars() failed: ") + mpg123_plain_strerror(errcode));
	mpg123_par(pars, MPG123_ADD_FLAGS, MPG123_QUIET, 0);

	handle = mpg123_parnew(pars, NULL, &errcode);
	if (handle == NULL)
		throw DecoderException(string("mpg123_new() failed: ") + mpg123_plain_strerror(errcode));

	if (mpg123_format(handle, 44100, MPG123_STEREO, MPG123_ENC_SIGNED_16) != MPG123_OK)
		throw DecoderException(string("mpg123_format() failed: ") + mpg123_strerror(handle));

	/*
	 * Register our own read/seek handlers and open the stream; we use our
	 * own indices in the decoders array, these aren't file descriptors!
	 */
	if (mpg123_replace_reader(handle, my_mpg123_read, my_mpg123_seek) != MPG123_OK)
		throw DecoderException(string("mpg123_replace_reader() failed: ") + mpg123_strerror(handle));

	if (mpg123_open_fd(handle, decoderidx) != MPG123_OK)
		throw DecoderException(string("mpg123_open_feed() failed: ") + mpg123_strerror(handle));

	/* Default to no ID3 information */
	v1 = NULL; v2 = NULL; handled_id3 = false;
	artist = NULL; album = NULL; title = NULL;
	year = 0; tracknr = 0;

	/*
	 * If the input is seekable, attempt to obtain ID3 tags.
	 */
	if (input->seek(input->getCurrentPosition()) && mpg123_scan(handle) == MPG123_OK)
		handleID3();
}

void
DecoderMP3_MPG123::run()
{
	/*
	 * For libmpg123, we read a chunk of data and ask it to decode. If we have
	 * read zero bytes and it has returned zero bytes of decoded output, we
	 * know that the MP3 has finished and all buffers are empty.
	 */
	size_t decoded = 0;
	size_t len;
	int err;
	while (!terminating) {
		handlePause();

		err = mpg123_read(handle, (unsigned char*)out_buffer, DECODER_OUTBUF_SIZE, &decoded);

		if (!handled_id3) {
			handleID3();
			handled_id3 = true;
		}

		/*	
		 * mpg123_decode() returns more a status than an error: if it wants more
		 * data, we'll give it to it the next run (but we must play it anyway
		 * since this may be the last part of the song - it may not know this
		 * because we are streaming data to it)
		 */
		if (err != MPG123_OK && err != MPG123_NEW_FORMAT && err != MPG123_DONE)
			throw DecoderException(string("mpg123_decode() failed: ") + mpg123_strerror(handle));

		/*
		 * Update playing/total playing times; playing time is in frames, so if we
		 * divide by 1 / time_per_frame, we get it in seconds. The total length
		 * is in samples; our sample rate is 44.1kHz so we just need to divide by
		 * that to get the number of seconds.
		 */
		playingtime = mpg123_tellframe(handle) / ((float)1.0f / mpg123_tpf(handle));
		totaltime = mpg123_length(handle) / 44100;

		if (decoded == 0)
			continue;
		if (visualizer != NULL)
			visualizer->update(out_buffer, decoded);
		if (output != NULL)
			output->play(out_buffer, decoded);
		if (err == MPG123_DONE)
			break;
	}
}

DecoderMP3_MPG123::~DecoderMP3_MPG123()
{
	/* Remove ourselves from the decoder list */
	pthread_mutex_lock(&libmpg123_mutex);
	for (unsigned int decoderidx = 0; decoderidx < libmpg123_numdecoders; decoderidx++)
		if (libmpg123_decoder[decoderidx] == this) {
			libmpg123_decoder[decoderidx] = NULL;
			break;
		}
	pthread_mutex_unlock(&libmpg123_mutex);

	mpg123_delete(handle);
	mpg123_delete_pars(pars);
	if (artist != NULL) free(artist);
	if (album  != NULL) free(album);
	if (title  != NULL) free(title);
}

list<string> 
DecoderMP3_MPG123::getExtensions()
{
	list<string> l;
	l.push_back("mp3");
	return l;
}

bool
DecoderMP3_MPG123::handleID3()
{
	if (!mpg123_meta_check(handle) & MPG123_ID3)
		return false;
	if (mpg123_id3(handle, &v1, &v2) != MPG123_OK)
		return false;

	/* We have tags! Time to dissect them */
	if (v2 != NULL) {
		/* Prefer ID3v2 tags; these have no size restrictions */
#define FILL_V2_TAG(x) \
		if (v2->x != NULL && v2->x->fill) \
			x = strdup(v2->x->p);
		FILL_V2_TAG(artist);
		FILL_V2_TAG(album);
		FILL_V2_TAG(title);
		if (v2->year != NULL && v2->year->fill) {
			year = atoi(v2->year->p);
		}
		/*
		 * libmpg123 does not support 'track#' tags, so we have to look for it
		 * ourselves...
		 */
		for (unsigned int i = 0; i < v2->texts; i++) {
			if (v2->text[i].text.p == NULL)
				continue;
			if (v2->text[i].id[0] == 'T' && v2->text[i].id[1] == 'R' &&
					v2->text[i].id[2] == 'C' && v2->text[i].id[3] == 'K') {
				/* TRCK - ID3v2 Track number/Position in set */
				tracknr = atoi(v2->text[i].text.p);
			}
		}
	} else if (v1 != NULL) {
		/* Grr - stupid libmpg123 doesn't nul-terminate the ID3v1 tags... */
		artist = (char*)malloc(32); album = (char*)malloc(32); title = (char*)malloc(32);
		artist[31] = '\0'; album[31] = '\0'; title[31] = '\0';
		memcpy(artist, v1->artist, sizeof(v1->artist));
		memcpy(album,  v1->album,  sizeof(v1->album));
		memcpy(title,  v1->title,  sizeof(v1->title));
		char tmp[32]; tmp[31] = '\0';
		memcpy(tmp, v1->year, sizeof(v1->year));
		year = atoi(tmp);
	}
	return true;
}

/* vim:set ts=2 sw=2: */
