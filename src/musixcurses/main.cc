#include <sys/types.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <getopt.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include "core/folderfactory.h"
#include "core/outputmixerfactory.h"
#include "core/exceptions.h"
#include "misc/configuration.h"
#include "interface.h"

using namespace std;

#define DEFAULT_CONFIG_FILE ".musixboxrc"

Interface* interface = NULL;
Configuration* config;

void
handle_update(int num)
{
	if (interface != NULL)
		interface->requestUpdate();
	signal(SIGALRM, handle_update);
}

void
handle_resize(int num)
{
	if (interface != NULL)
		interface->signalResize();
	signal(SIGWINCH, handle_resize);
}

void
usage()
{
	fprintf(stderr, "usage: musixcurses [-?hn] [-o type] [-p path] [resource]\n\n");
	fprintf(stderr, " -h, -?         this help\n");
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
		Folder* folder = NULL;
		Output* output = NULL;
		Mixer* mixer;
		int c;

		while ((c = getopt(argc, argv, "?hnc:o:p:")) != -1) {
			switch(c) {
				case 'h':
				case '?': usage();
					  /* NOTREACHED */
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
			}
		}
		argc -= optind;
		argv += optind;

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

		signal(SIGALRM, handle_update);
		signal(SIGWINCH, handle_resize);

		interface = new Interface(output, mixer, folder, (argc > 0) ? argv[0] : NULL);
		interface->run();

		/* Only store the configuration on successful termination */
		config->store();

		delete config;
		delete interface;
		delete output;
		delete folder;
	} catch (MusixBoxException& e) {
		fprintf(stderr, "%s\n", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
