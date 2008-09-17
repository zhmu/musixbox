#include "config.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
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
	fprintf(stderr, "usage: musixbox [-?hn"
#ifdef WITH_SDL
"s"
#endif
"] [-c config] [-a device] [-o type] [resource]\n\n");
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
	fprintf(stderr, "\n");
	fprintf(stderr, " -p path        path to media files\n");
	fprintf(stderr, " -c config      location of configuration file\n");
	fprintf(stderr, "                default: ~/%s\n", DEFAULT_CONFIG_FILE);
	fprintf(stderr, " -n             clear configuration file\n");
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
		while ((ch = getopt(argc, argv, "?hn"
#ifdef WITH_SDL
"s"
#endif
"a:c:o:p:")) != -1) {
		switch(ch) {
#ifdef WITH_SDL
			case 's': interaction->add(new InteractionSDL());
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
				interaction->add(new InteractionSDL());
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
