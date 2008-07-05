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
	fprintf(stderr, " -a device      enable AVR interaction frontend using device\n");
	fprintf(stderr, " -o type	 select output plugin\n");
	fprintf(stderr, "                available are: null ao\n\n");
	fprintf(stderr, "folder is where your media files are expected to be\n");
	exit(EXIT_SUCCESS);
}

Output*
findOutputProvider(const char* name)
{
	if (!strcmp(name, "null"))
		return new OutputNull();
	if (!strcmp(name, "ao"))
		return new OutputAO();
	fprintf(stderr, "no such output provider '%s'\n", name);
	usage();
}

int
main(int argc, char** argv)
{
	InteractionChain* interaction = new InteractionChain();
	Interface* interface;
	Output* output = NULL;
	int ch;

	while ((ch = getopt(argc, argv, "?hsa:o:")) != -1) {
		switch(ch) {
			case 's': interaction->add(new InteractionSDL());
			          break;
			case 'a': interaction->add(new InteractionAVR(optarg));
			          break;
			case 'o': output = findOutputProvider(optarg);
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
		return EXIT_FAILURE;
	}

	if (output == NULL) {
		fprintf(stderr, "fatal: no output provider, aborting\n");
		return EXIT_FAILURE;
	}
	if (!output->init()) {
		fprintf(stderr, "fatal: output provider didn't initialize correctly\n");
		return EXIT_FAILURE;
	}

	interface = new Interface(interaction, output, argv[0]);

	if (!interaction->init()) {
		fprintf(stderr, "interaction init fail\n");
		return EXIT_FAILURE;
	}
	interface->init();

	interface->run();

	interface->done();
	interaction->done();
	output->done();
	
	return EXIT_SUCCESS;
}
