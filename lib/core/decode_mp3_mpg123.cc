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
	
#define CHUNK_SIZE 8192

DecoderMP3_MPG123::DecoderMP3_MPG123(Player* p, Input* i, Output* o, Visualizer* v)
	: Decoder(p, i, o, v)
{
	/*
	 * libmpg123 wants to be initialized just once; this seems the best place.
	 */
	if (!libmpg123_initialized) {
		if (mpg123_init() != MPG123_OK)
			throw new DecoderException("mpg123_init() failed");
		libmpg123_initialized = true;
	}

	int errcode;
	handle = mpg123_new(NULL, &errcode);
	if (handle == NULL)
		throw new DecoderException(string("mpg123_new() failed: ") + mpg123_plain_strerror(errcode));

	if (mpg123_format(handle, 44100, MPG123_STEREO, MPG123_ENC_SIGNED_16) != MPG123_OK)
		throw new DecoderException(string("mpg123_format() failed: ") + mpg123_strerror(handle));

	if (mpg123_open_feed(handle) != MPG123_OK)
		throw new DecoderException(string("mpg123_open_feed() failed: ") + mpg123_strerror(handle));

	off_t file_len = input->getLength();
	if (file_len > 0) {
		mpg123_set_filesize(handle, file_len);
	}
	v1 = NULL; v2 = NULL; handled_id3 = false;
	artist = NULL; album = NULL; title = NULL;
	year = 0; tracknr = 0;
}

void
DecoderMP3_MPG123::run()
{
	char music_chunk[CHUNK_SIZE];

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

		len = input->read(music_chunk, CHUNK_SIZE);
		err = mpg123_decode(handle, (unsigned char*)music_chunk, len, (unsigned char*)out_buffer, DECODER_OUTBUF_SIZE, &decoded);
		if ((len == 0 && decoded == 0) || err == MPG123_DONE)
			break;

		if (!handled_id3) {
			if (mpg123_meta_check(handle) & MPG123_ID3 && mpg123_id3(handle, &v1, &v2) == MPG123_OK) {
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
			}
			handled_id3 = true;
		}

		/*	
		 * mpg123_decode() returns more a status than an error: if it wants more
		 * data, we'll give it to it the next run (but we must play it anyway
		 * since this may be the last part of the song - it may not know this
		 * because we are streaming data to it)
		 */
		if (err != MPG123_OK && err != MPG123_NEW_FORMAT && err != MPG123_NEED_MORE)
			throw new DecoderException(string("mpg123_decode() failed: ") + mpg123_strerror(handle));

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
	}
}

DecoderMP3_MPG123::~DecoderMP3_MPG123()
{
	mpg123_delete(handle);
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

/* vim:set ts=2 sw=2: */
