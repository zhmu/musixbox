#include "config.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "exceptions.h"
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
#include "mixer_oss.h"

InteractionChain* interaction;
Interface* interface;
Output* output;
Mixer* mixer;

void
usage()
{
	fprintf(stderr, "usage: musixbox [-?hs] [-a device] folder [resource]\n\n");
	fprintf(stderr, " -h, -?         this help\n");
#ifdef WITH_SDL
	fprintf(stderr, " -s             enable SDL interaction frontend\n");
#endif
	fprintf(stderr, " -a device      enable AVR interaction frontend using device\n");
	fprintf(stderr, " -m device      specify mixer device, defaults to /dev/mixer0\n");
	fprintf(stderr, " -o type        select output plugin\n");
	fprintf(stderr, "                available are: null");
#ifdef WITH_AO
	fprintf(stderr, " ao");
#endif
	fprintf(stderr, "\n\n");
	fprintf(stderr, "folder is where your media files are expected to be\n");
	fprintf(stderr, "resource is optional; if specified, musixbox will immediately begin to play it\n");
	exit(EXIT_SUCCESS);
}

void
terminate(int)
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
	std::string mixdev = "/dev/mixer0";

	interface = NULL;
	output = NULL;
	mixer = NULL;
	interaction = new InteractionChain();

	try {
		while ((ch = getopt(argc, argv, "?h"
#ifdef WITH_SDL
"s"
#endif
"a:o:"
"m:")) != -1) {
		switch(ch) {
#ifdef WITH_SDL
			case 's': interaction->add(new InteractionSDL());
			          break;
#endif
			case 'a': interaction->add(new InteractionAVR(optarg));
			          break;
			case 'o': output = findOutputProvider(optarg);
			          break;
			case 'm': mixdev = std::string(optarg); 
			          break;
			case 'h':
			case '?': usage();
			          /* NOTREACHED */
			}
		}
		argc -= optind;
		argv += optind;
		if (argc < 1) {
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
		mixer = new MixerOSS(mixdev);
		interface = new Interface(interaction, output, mixer, argv[0], (argc > 1) ? argv[1] : NULL);

		signal(SIGINT, terminate);
		signal(SIGTERM, terminate);

		interface->run();
	} catch (MusixBoxException& e) {
		fprintf(stderr, "%s\n", e.what());
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
