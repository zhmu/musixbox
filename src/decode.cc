#include <stdlib.h>
#include "decode.h"

Decoder::Decoder() {
	output = NULL; visualizer = NULL;
	out_buffer = (char*)malloc(DECODER_OUTBUF_SIZE);
}

Decoder::~Decoder() {
	free(out_buffer);
}
