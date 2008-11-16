#include <stdlib.h>
#include "decode.h"
#include "player.h"

using namespace std;

Decoder::Decoder(Player* p, Input* i, Output* o, Visualizer* v) {
	player = p; input = i; output = o; visualizer = v; terminating = false;
	out_buffer = (char*)malloc(DECODER_OUTBUF_SIZE);
	playingtime = 0; totaltime = 0;
}

Decoder::~Decoder() {
	free(out_buffer);
}

void
Decoder::handlePause()
{
	player->handleUnpause();
}

std::list<std::string> 
Decoder::getExtensions()
{
	return std::list<std::string> ();
}
