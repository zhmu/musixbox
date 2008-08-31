#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode_flac.h"
#include "exceptions.h"
#include "input.h"
#include "output.h"
#include "visualize.h"

DecoderFLAC::DecoderFLAC(Input* i, Output* o, Visualizer* v)
	: Decoder(i, o, v)
{
	artist = NULL; album = NULL; title = NULL;

	set_metadata_respond(FLAC__METADATA_TYPE_VORBIS_COMMENT);
	init();
}

DecoderFLAC::~DecoderFLAC()
{
	if (artist != NULL)
		free(artist);
	if (album != NULL)
		free(album);
	if (title != NULL)
		free(title);
}

int
DecoderFLAC::run()
{
	while (!terminating && process_single()) {
		/* keep rolling */
	}

	return 0;
}

FLAC__StreamDecoderReadStatus
DecoderFLAC::read_callback(FLAC__byte buffer[], size_t* bytes)
{
	size_t size = input->read((char*)buffer, *bytes);
	if (size == 0)
		return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
	if (size < 0)
		return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
	*bytes = size;
	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderWriteStatus
DecoderFLAC::write_callback(const FLAC__Frame* frame, const FLAC__int32 *const buffer[])
{
	size_t out = 0;

	playingtime = frame->header.number.sample_number / frame->header.sample_rate;

	for (unsigned int i = 0; i < frame->header.blocksize; i++) {
		out_buffer[4 * out    ] = (buffer[0][i]     ) & 0xff;
		out_buffer[4 * out + 1] = (buffer[0][i] >> 8) & 0xff;
		out_buffer[4 * out + 2] = (buffer[1][i]     ) & 0xff;
		out_buffer[4 * out + 3] = (buffer[1][i] >> 8) & 0xff;
		out++;
		if (out == DECODER_OUTBUF_SIZE / 4) {
			output->play(out_buffer, out * 4);
			out = 0;
		}
	}
	if (out > 0) {
		output->play(out_buffer, out * 4);
	}
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void
DecoderFLAC::error_callback(FLAC__StreamDecoderErrorStatus status)
{
	switch(status) {
		         case FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC: throw DecoderException(std::string("DecoderFLAC: Synchronization lost"));
		        case FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER: throw DecoderException(std::string("DecoderFLAC: Corrupted header encountered"));
		case FLAC__STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH: throw DecoderException(std::string("DecoderFLAC: CRC error"));
		case FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM: throw DecoderException(std::string("DecoderFLAC: Unparsable stream"));
	}
	throw DecoderException(std::string("DecoderFLAC: unknown error"));
}

void
DecoderFLAC::metadata_callback(const FLAC__StreamMetadata* metadata)
{
	if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
		/*
		 * Stream info block; we can calculate the song lengtrh
		 * using this information.
		 */
		totaltime = (int)(metadata->data.stream_info.total_samples / metadata->data.stream_info.sample_rate);
		return;
	}

	if (metadata->type == FLAC__METADATA_TYPE_VORBIS_COMMENT) {
		for (unsigned int i = 0; i < metadata->data.vorbis_comment.num_comments; i++) {
			char* tag = (char*)metadata->data.vorbis_comment.comments[i].entry;
			char* ptr = strchr(tag, '=');
			if (ptr == NULL)
				continue;

			if (!strncasecmp(tag, "TITLE", ptr - tag)) {
				title = strdup(ptr + 1);
			}
			if (!strncasecmp(tag, "ARTIST", ptr - tag)) {
				artist = strdup(ptr + 1);
			}
			if (!strncasecmp(tag, "ALBUM", ptr - tag)) {
				album = strdup(ptr + 1);
			}
		}
	}
}
