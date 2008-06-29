#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mad.h>
#include <math.h>
#include <id3/tag.h>
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
	char* ptr = music_chunk;
	char* base = music_chunk;

	if (stream->next_frame != NULL) {
		/*
		 * There is still data in the buffer that should be played, so
		 * we need to recycle it.
		 */
		saved = stream->bufend - stream->next_frame;
		memmove(ptr, stream->next_frame, saved);
		ptr += saved;
	} else {
		saved = 0;
	}

	len = input->read(ptr, CHUNK_SIZE - saved);
	if (!len)
		return 0;

	mad_stream_buffer(stream, (unsigned char*)base, len + saved);
	return 1;
}

DecoderMP3::DecoderMP3(Input* i, Output* o, Visualizer* v) : Decoder(i, o, v)
{
	music_chunk = (char*)malloc(CHUNK_SIZE);
}

DecoderMP3::~DecoderMP3()
{
	free(music_chunk);
}

int
DecoderMP3::run()
{
	struct mad_stream stream;
	struct mad_frame frame;
	struct mad_synth synth;
	struct mad_header header;
	int buflen;
	mad_timer_t time = mad_timer_zero;

	mad_stream_init(&stream);
	mad_frame_init(&frame);
	mad_synth_init(&synth);
	mad_stream_options(&stream, 0);

	/*
	 * Scan the entire file until we find a header we can understand. 
	 */
	do {
		if (!terminating && !handleInput(&stream))
			goto fail;

		if (mad_header_decode(&header, &stream) < 0) {
			if (MAD_RECOVERABLE(stream.error))
				continue;
			break;
		}
		/* We found a header - stop here */
		break;
	} while (1);
	
	if (!MAD_RECOVERABLE(stream.error))
		goto fail;

	do {
		if (!terminating && !handleInput(&stream))
			goto fail;

		while (!terminating) {
			if (mad_frame_decode(&frame, &stream) == -1) {
				if (!MAD_RECOVERABLE(stream.error))
					break;
				continue;
			}
			mad_timer_add(&time, frame.header.duration);
			playingtime = time.seconds;

			mad_synth_frame(&synth, &frame);
			buflen = DECODER_OUTBUF_SIZE;
			if (!convertOutput(out_buffer, &buflen, &frame.header, &synth.pcm))
				goto fail;

			if (visualizer != NULL)
				visualizer->update(out_buffer, buflen);
			if (output != NULL)
				output->play(out_buffer, buflen * 4);
		}
	} while (stream.error == MAD_ERROR_BUFLEN && !terminating);

fail:
	mad_synth_finish(&synth);
	mad_frame_finish(&frame);
	mad_stream_finish(&stream);	
	free (music_chunk);

	return 1;
}
