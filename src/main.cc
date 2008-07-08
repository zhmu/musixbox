#include "config.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "interface.h"
#ifdef WITH_SDL
#include "interaction_sdl.h"
#endif
#include "interaction_avr.h"
#include "interaction_chain.h"
#ifdef WITH_AO
#include "output_ao.h"
#endif
#include "output_null.h"

InteractionChain* interaction;
Interface* interface;
Output* output;

void
usage()
{
	fprintf(stderr, "usage: musicd [-?hs] [-a device] folder\n\n");
	fprintf(stderr, " -h, -?         this help\n");
#ifdef WITH_SDL
	fprintf(stderr, " -s             enable SDL interaction frontend\n");
#endif
	fprintf(stderr, " -a device      enable AVR interaction frontend using device\n");
	fprintf(stderr, " -o type        select output plugin\n");
	fprintf(stderr, "                available are: null");
#ifdef WITH_AO
	fprintf(stderr, " ao");
#endif
	fprintf(stderr, "\n\n");
	fprintf(stderr, "folder is where your media files are expected to be\n");
	exit(EXIT_SUCCESS);
}

void
sigint(int)
{
	interaction->requestTermination();
}

Output*
findOutputProvider(const char* name)
{
	if (!strcmp(name, "null"))
		return new OutputNull();
#ifdef WITH_AO
	if (!strcmp(name, "ao"))
		return new OutputAO();
#endif
	fprintf(stderr, "no such output provider '%s'\n", name);
	usage();

	/* NOTREACHED */
	return NULL;
}

int
main(int argc, char** argv)
{
	int ch;

	interface = NULL;
	output = NULL;
	interaction = new InteractionChain();

	while ((ch = getopt(argc, argv, "?h"
#ifdef WITH_SDL
"s"
#endif
"a:o:")) != -1) {
		switch(ch) {
#ifdef WITH_SDL
			case 's': interaction->add(new InteractionSDL());
			          break;
#endif
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

	signal(SIGINT, sigint);

	interface->run();

	interface->done();
	interaction->done();
	output->done();
	
	return EXIT_SUCCESS;
}
