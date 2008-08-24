#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#include "exceptions.h"
#include "decode_ogg.h"
#include "input.h"
#include "output.h"

size_t
ov_wrap_read(void* ptr, size_t size, size_t nmemb, void* datasource)
{
	Input* input = (Input*)datasource;
	return (input->read((char*)ptr, size * nmemb) / size);
}

ov_callbacks ov_input_wrapper = {
	ov_wrap_read,
	NULL,
	NULL,
	NULL
};

DecoderOgg::~DecoderOgg()
{
	ov_clear(&ovf);
}

DecoderOgg::DecoderOgg(Input* i, Output* o, Visualizer* v) :
	Decoder(i, o, v)
{
	if (ov_open_callbacks(input, &ovf, NULL, 0, ov_input_wrapper) < 0)
		throw DecoderException("ov_open_callbacks() failed");
}

int
DecoderOgg::run()
{
	int current;

	totaltime = (int)ov_time_total(&ovf, 0);
	if (totaltime < 0)
		totaltime = 0;

	while (!terminating) {
		size_t len = ov_read(&ovf, out_buffer, DECODER_OUTBUF_SIZE, 0, 2, 1, &current);
		if (len <= 0)
			/* end of file or error */
			break;

		playingtime = (int)ov_time_tell(&ovf);

		if (visualizer != NULL)
			visualizer->update(out_buffer, len);
		if (output != NULL)
			output->play(out_buffer, len);
	}

	return 1;
}

struct vorbis_comment*
DecoderOgg::getComments()
{
	return ov_comment(&ovf, -1);
}
