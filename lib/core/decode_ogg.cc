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

int
ov_wrap_seek(void* datasource, ogg_int64_t offset, int whence)
{
	Input* input = (Input*)datasource;
	switch (whence) {
		case SEEK_CUR: /* seek from current position */
		               offset += input->getCurrentPosition();
		               break;
		case SEEK_END: /* seek from endposition */
		               offset = input->getLength() - offset;
		               break;
		case SEEK_SET: /* go there, now */
		               break;
		      default: /* ??? */
		               return -1;
		               
	}
	return input->seek(offset) ? 0 : -1;
}

long
ov_wrap_tell(void* datasource)
{
	Input* input = (Input*)datasource;
	return input->getCurrentPosition();
}

ov_callbacks ov_input_wrapper = {
	ov_wrap_read,
	ov_wrap_seek,
	NULL,
	ov_wrap_tell
};

DecoderOgg::~DecoderOgg()
{
	ov_clear(&ovf);
}

DecoderOgg::DecoderOgg(Input* i, Output* o, Visualizer* v) :
	Decoder(i, o, v)
{
	if (ov_open_callbacks(input, &ovf, NULL, 0, ov_input_wrapper) < 0)
		throw DecoderException(std::string("ov_open_callbacks() failed"));
}

void
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
}

struct vorbis_comment*
DecoderOgg::getComments()
{
	return ov_comment(&ovf, -1);
}
