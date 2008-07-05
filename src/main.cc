#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "decode_mp3.h"
#include "decode_ogg.h"
#include "decode_tone.h"
#include "input_file.h"
#include "interface.h"
#include "interaction_sdl.h"
#include "interaction_chain.h"
#include "output_ao.h"
#include "output_null.h"
#include "visualize_spectrum.h"

int
main(int argc, char** argv)
{
	InteractionChain* interaction;
	Interface* interface;

	interaction = new InteractionChain();
	interaction->add(new InteractionSDL());
	interface = new Interface(interaction);

	if (!interaction->init()) {
		fprintf(stderr, "interaction init fail\n");
		return 1;
	}
	interface->init();

	interface->run();

	interface->done();
	interaction->done();
	
	return 0;
}
