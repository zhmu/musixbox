#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "decode_tone.h"
#include "output.h"

void
DecoderTone::run()
{
	while (1) {
		int buflen = DECODER_OUTBUF_SIZE / 4;
		for (int i = 0; i < buflen; i++) {
			int sample = (int)(0.75 * 32768.0 * 
				sin(2 * M_PI * freq * ((float) i/44100)));
			
			/* Put the same stuff in left and right channel */
			out_buffer[4*i] = out_buffer[4*i+2] = sample & 0xff;
			out_buffer[4*i+1] = out_buffer[4*i+3] = (sample >> 8) & 0xff;
		}

		if (visualizer != NULL)
			visualizer->update(out_buffer, buflen);
		if (output != NULL)
			output->play(out_buffer, buflen);
	}
}
