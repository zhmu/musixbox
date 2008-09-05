#include "config.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "core/exceptions.h"
#include "core/folderfactory.h"
#include "core/outputmixerfactory.h"
#include "interface.h"
#ifdef WITH_SDL
#include "ui/interaction_sdl.h"
#endif
#include "ui/interaction_avr.h"
#include "ui/interaction_chain.h"

using namespace std;

InteractionChain* interaction;

void
usage()
{
	fprintf(stderr, "usage: musixbox [-?h"
#ifdef WITH_SDL
"s"
#endif
"] [-a device] [-o type] folder [resource]\n\n");
	fprintf(stderr, " -h, -?         this help\n");
#ifdef WITH_SDL
	fprintf(stderr, " -s             enable SDL interaction frontend\n");
#endif
	fprintf(stderr, " -a device      enable AVR interaction frontend using device\n");
	fprintf(stderr, " -o type        select output plugin\n");
	fprintf(stderr, "                available are:");
	list<string> l;
	OutputMixerFactory::getAvailable(l);
	for (list<string>::iterator it = l.begin(); it != l.end(); it++) {
		fprintf(stderr, " %s", (*it).c_str());
	}
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

int
main(int argc, char** argv)
{
	try {
		Output* output = NULL;
		Interface* interface;
		Folder* folder;
		Mixer* mixer;
		InteractionChain* interaction = new InteractionChain();

		int ch;
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
			case 'o': OutputMixerFactory::construct(optarg, &output, &mixer);
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
		FolderFactory::construct(argv[0], &folder);
		interface = new Interface(interaction, output, mixer, folder, (argc > 1) ? argv[1] : NULL);

		signal(SIGINT, terminate);
		signal(SIGTERM, terminate);

		interface->run();

		delete interface;
		delete output;
		delete folder;
	} catch (MusixBoxException& e) {
		fprintf(stderr, "%s\n", e.what());
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
