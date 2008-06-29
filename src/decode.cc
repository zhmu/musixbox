#include <stdlib.h>
#include "decode.h"

Decoder::Decoder(Input* i, Output* o, Visualizer* v) {
	input = i; output = o; visualizer = v; terminating = false;
	out_buffer = (char*)malloc(DECODER_OUTBUF_SIZE);
	playingtime = 0; totaltime = 0;
}

Decoder::~Decoder() {
	free(out_buffer);
}
