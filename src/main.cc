#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "decode_mp3.h"
#include "decode_ogg.h"
#include "decode_tone.h"
#include "input_file.h"
#include "interface.h"
#include "interaction_sdl.h"
#include "output_ao.h"
#include "output_null.h"
#include "visualize_spectrum.h"

int
main(int argc, char** argv)
{
	Interaction* interaction;
	Interface* interface;

	interaction = new InteractionSDL();
	interface = new Interface(interaction);

	interaction->init();
	interface->init();

	interface->run();

	interface->done();
	interaction->done();
	
	return 0;
}
