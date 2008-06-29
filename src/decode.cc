#include <stdlib.h>
#include "decode.h"

Decoder::Decoder() {
	input = NULL; output = NULL; visualizer = NULL; terminating = false;
	out_buffer = (char*)malloc(DECODER_OUTBUF_SIZE);
	playingtime = 0; totaltime = 0;
}

Decoder::~Decoder() {
	free(out_buffer);
}
