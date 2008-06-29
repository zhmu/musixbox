#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
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

int
DecoderOgg::run()
{
	struct OggVorbis_File ovf;
	int current;

	if (ov_open_callbacks(input, &ovf, NULL, 0, ov_input_wrapper) < 0)
		return 0;

	while (1) {
		size_t len = ov_read(&ovf, out_buffer, DECODER_OUTBUF_SIZE, 0, 2, 1, &current);
		if (len <= 0)
			/* end of file or error */
			break;

		if (visualizer != NULL)
			visualizer->update(out_buffer, len);
		if (output != NULL)
			output->play(out_buffer, len);
	}
	ov_clear(&ovf);

	return 1;
}
