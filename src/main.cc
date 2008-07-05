#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "decode_mp3.h"
#include "decode_ogg.h"
#include "decode_tone.h"
#include "input_file.h"
#include "interface.h"
#include "interaction_sdl.h"
#include "interaction_avr.h"
#include "interaction_chain.h"
#include "output_ao.h"
#include "output_null.h"
#include "visualize_spectrum.h"

void
usage()
{
	fprintf(stderr, "usage: musicd [-?hs] [-a device] folder\n\n");
	fprintf(stderr, " -h, -?         this help\n");
	fprintf(stderr, " -s             enable SDL interaction frontend\n");
	fprintf(stderr, " -a device      enable AVR interaction frontend using device\n\n");
	fprintf(stderr, "folder is where your media files are expected to be\n");
	exit(EXIT_SUCCESS);
}

int
main(int argc, char** argv)
{
	InteractionChain* interaction = new InteractionChain();
	Interface* interface;
	int ch;

	while ((ch = getopt(argc, argv, "?hsa:")) != -1) {
		switch(ch) {
			case 's': interaction->add(new InteractionSDL());
			          break;
			case 'a': interaction->add(new InteractionAVR(optarg));
			          break;
			case 'h':
			case '?': usage();
			          /* NOTREACHED */
		}
	}
	argc -= optind;
	argv += optind;
	if (argc != 1) {
		fprintf(stderr, "error: no media path given\n");
		usage();
	}

	if (interaction->getNumProviders() == 0) {
		fprintf(stderr, "fatal: no interaction providers, aborting\n");
		return 1;
	}

	interface = new Interface(interaction, argv[0]);

	if (!interaction->init()) {
		fprintf(stderr, "interaction init fail\n");
		return 1;
	}
	interface->init();

	interface->run();

	interface->done();
	interaction->done();
	
	return EXIT_SUCCESS;
}
