#include "config.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include "core/exceptions.h"
#include "core/folderfactory.h"
#include "core/outputmixerfactory.h"
#include "misc/configuration.h"
#include "interface.h"
#ifdef WITH_SDL
#include "ui/interaction_sdl.h"
#endif
#include "ui/interaction_avr.h"
#include "ui/interaction_chain.h"

using namespace std;

#define DEFAULT_CONFIG_FILE ".musixboxrc"

InteractionChain* interaction;
Configuration* config;

void
usage()
{
	fprintf(stderr, "usage: musixbox [-?dhn"
#ifdef WITH_SDL
"s"
#endif
"] " 
#ifdef WITH_SDLGFX
"[-z factor] " 
#endif
"[-c config] [-a device] [-o type] [resource]\n\n");
	fprintf(stderr, " -h, -?         this help\n");
#ifdef WITH_SDL
	fprintf(stderr, " -s             enable SDL interaction frontend\n");
#ifdef WITH_SDLGFX
	fprintf(stderr, " -z factor      set SDL zoom factor\n");
	fprintf(stderr, "                (not stored in configuration!)\n");
#endif
#endif
	fprintf(stderr, " -a device      enable AVR interaction frontend using device\n");
	fprintf(stderr, " -o type        select output plugin\n");
	fprintf(stderr, "                available are:");
	list<string> l;
	OutputMixerFactory::getAvailable(l);
	for (list<string>::iterator it = l.begin(); it != l.end(); it++) {
		fprintf(stderr, " %s", (*it).c_str());
	}
	fprintf(stderr, "\n");
	fprintf(stderr, " -p path        path to media files\n");
	fprintf(stderr, " -c config      location of configuration file\n");
	fprintf(stderr, "                default: ~/%s\n", DEFAULT_CONFIG_FILE);
	fprintf(stderr, " -n             clear configuration file\n");
	fprintf(stderr, " -d             daemonize after startup\n");
	fprintf(stderr, "\n");
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
	string cfgfile;
	int daemonize = 0;
#ifdef WITH_SDL
	int zoom_factor = 1;
#endif

	if (getenv("HOME") != NULL)
		cfgfile = string(getenv("HOME")) + "/"DEFAULT_CONFIG_FILE;
	else
		cfgfile = DEFAULT_CONFIG_FILE;
	config = new Configuration(cfgfile);
	try {
		Output* output = NULL;
		Interface* interface;
		Folder* folder = NULL;
		Mixer* mixer;
		interaction = new InteractionChain();

		int ch;
		while ((ch = getopt(argc, argv, "?dhn"
#ifdef WITH_SDL
"s"
#ifdef WITH_SDLGFX
"z:"
#endif
#endif
"a:c:o:p:")) != -1) {
		switch(ch) {
#ifdef WITH_SDL
			case 's': interaction->add(new InteractionSDL(zoom_factor));
			          config->setString("SDL interaction", "yes");
			          break;
#endif
			case 'a': interaction->add(new InteractionAVR(optarg));
			          /* If we got here, this worked - so store the provider */
			          config->setString("avr interaction", optarg);
			          break;
			case 'o': OutputMixerFactory::construct(optarg, &output, &mixer);
			          /* If we got here, this worked - so store the provider */
			          config->setString("output provider", optarg);
			          break;
			case 'p': FolderFactory::construct(optarg, &folder);
			          /* If we got here, this worked - so store the path */
			          config->setString("media path", optarg);
			          break;
			case 'c': delete config;
			          config = new Configuration(optarg);
			          break;
			case 'n': config->clear();
			          break;
			case 'd': daemonize++;
			          break;
#ifdef WITH_SDLGFX
			case 'z': zoom_factor = atoi(optarg);
			          if (zoom_factor <= 0)
			          	throw MusixBoxException("Invalid zoom factor");
			          break;
#endif
			case 'h':
			case '?': usage();
			          /* NOTREACHED */
			}
		}
		argc -= optind;
		argv += optind;

		if (interaction->getNumProviders() == 0) {
			string a = config->getString("avr interaction", "");
			if (a != "")
				interaction->add(new InteractionAVR(a.c_str()));
#ifdef WITH_SDL
			if (config->getString("SDL interaction", "") != "")
				interaction->add(new InteractionSDL(zoom_factor));
#endif
			if (interaction->getNumProviders() == 0) {
				fprintf(stderr, "fatal: no interaction providers, aborting\n");
				return EXIT_FAILURE;
			}
		}

		if (output == NULL) {
			string str = config->getString("output provider", "");
			if (str == "") {
				fprintf(stderr, "fatal: no output provider, aborting\n");
				return EXIT_FAILURE;
			}
			OutputMixerFactory::construct(str, &output, &mixer);
		}

		if (folder == NULL) {
			string str = config->getString("media path", "");
			if (str == "") {
				fprintf(stderr, "fatal: no media path provided, aborting\n");
				return EXIT_FAILURE;
			}
			FolderFactory::construct(str, &folder);
		}

		interface = new Interface(interaction, output, mixer, folder, (argc > 0) ? argv[0] : NULL);

		signal(SIGINT, terminate);
		signal(SIGTERM, terminate);

		if (daemonize && daemon(1, 1) < 0)
			fprintf(stderr, "couldn't daemonize (%s), continuing anyway\n", strerror(errno));

		interface->run();
	
		/* Only store the configuration on successful termination */
		config->store();

		delete interface;
		delete output;
		delete folder;
		delete interaction;
		delete config;
	} catch (MusixBoxException& e) {
		fprintf(stderr, "%s\n", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
