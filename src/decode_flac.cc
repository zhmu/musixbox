#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode_flac.h"
#include "input.h"
#include "output.h"
#include "visualize.h"

DecoderFLAC::DecoderFLAC(Input* i, Output* o, Visualizer* v)
	: Decoder(i, o, v)
{
	set_metadata_respond(FLAC__METADATA_TYPE_VORBIS_COMMENT);
	init();
}

DecoderFLAC::~DecoderFLAC()
{
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

	for (int i = 0; i < frame->header.blocksize; i++) {
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
	/* TODO: deal with errors in some way */
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
		/* TODO: deal with this */
	}
}
