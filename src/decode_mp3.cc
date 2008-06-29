#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mad.h>
#include <math.h>
#include "decode_mp3.h"
#include "output.h"

signed int
DecoderMP3::scaleFrequency(mad_fixed_t s)
{
	s += (1L << (MAD_F_FRACBITS - 16));
	if (s > MAD_F_ONE)
		s = MAD_F_ONE - 1;
	else if (s < -MAD_F_ONE)
		s = -MAD_F_ONE;
	return s >> (MAD_F_FRACBITS + 1 - 16);
}

int
DecoderMP3::convertOutput(char* buf, int* len, struct mad_header const* header, struct mad_pcm* pcm)
{
	unsigned int num = pcm->length;
	mad_fixed_t const *left, *right;
	int i;

	left = pcm->samples[0];
	right = pcm->samples[1];

	/* Ensure the output buffer is large enough */
	if (*len < num * 4)
		return 0;

	/*
	 * Convert 24 bit data to 16 bit suitable for output.
	 */
	i = 0;
	while (num--) {
		signed int s;
		s = scaleFrequency(*left++);
		buf[4 * i    ] = s & 0xff;
		buf[4 * i + 1] = (s >> 8) & 0xff;
		if (pcm->channels > 1)
			s = scaleFrequency(*right++);
		buf[4 * i + 2] = s & 0xff;
		buf[4 * i + 3] = (s >> 8) & 0xff;

		i++;
	}
	*len = i;
	
	return 1;
}


int
DecoderMP3::handleInput(struct mad_stream* stream)
{
	size_t len, max, saved;
	unsigned char* ptr = (unsigned char*)music_chunk;
	unsigned char* base = (unsigned char*)music_chunk;

	if (stream->next_frame != NULL) {
		/*
		 * There is still data in the buffer that should be played, so
		 * we need to recycle it.
		 */
		saved = (char*)stream->bufend - (char*)stream->next_frame;
		memmove(ptr, stream->next_frame, saved);
		ptr += saved;
	} else {
		saved = 0;
	}

	len = fread(ptr, 1, CHUNK_SIZE - saved, musicfile);
	if (!len)
		return 0;

	mad_stream_buffer(stream, base, len + saved);
	return 1;
}

DecoderMP3::DecoderMP3() : Decoder()
{
	musicfile = NULL;
	music_chunk = (char*)malloc(CHUNK_SIZE);
}

DecoderMP3::~DecoderMP3()
{
	free(music_chunk);
}

int
DecoderMP3::open(const char* fname)
{
	musicfile = fopen(fname, "rb");
	if (musicfile == NULL)
		return 0;

	return 1;
}

int
DecoderMP3::close()
{
	fclose(musicfile);
	return 1;
}

int
DecoderMP3::run()
{
	struct mad_stream stream;
	struct mad_frame frame;
	struct mad_synth synth;
	int buflen;

	mad_stream_init(&stream);
	mad_frame_init(&frame);
	mad_synth_init(&synth);
	mad_stream_options(&stream, 0);

	do {
		if (!handleInput(&stream))
			goto fail;

		while (1) {
			if (mad_frame_decode(&frame, &stream) == -1) {
				if (!MAD_RECOVERABLE(stream.error))
					break;
				continue;
			}

			mad_synth_frame(&synth, &frame);
			buflen = DECODER_OUTBUF_SIZE;
			if (!convertOutput(out_buffer, &buflen, &frame.header, &synth.pcm))
				goto fail;

			if (visualizer != NULL)
				visualizer->update(out_buffer, buflen);
			if (output != NULL)
				output->play(out_buffer, buflen * 4);
		}
	} while (stream.error == MAD_ERROR_BUFLEN);

fail:
	mad_synth_finish(&synth);
	mad_frame_finish(&frame);
	mad_stream_finish(&stream);	
	free (music_chunk);

	return 1;
}
