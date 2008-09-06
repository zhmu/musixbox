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
#include "interface.h"

using namespace std;

Interface* interface = NULL;

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
	fprintf(stderr, "usage: musixcurses [-?h] [-o type] folder\n\n");
	fprintf(stderr, " -h, -?         this help\n");
	fprintf(stderr, " -o type        select output plugin\n");
	fprintf(stderr, "                available are:");
	list<string> l;
	OutputMixerFactory::getAvailable(l);
	for (list<string>::iterator it = l.begin(); it != l.end(); it++) {
		fprintf(stderr, " %s", (*it).c_str());
	}
	fprintf(stderr, "\n\n");
	fprintf(stderr, "folder is where your media files are expected to be\n");
	exit(EXIT_SUCCESS);
}

int
main(int argc, char** argv)
{
	try {
		Folder* folder;
		Output* output;
		Mixer* mixer;
		int c;

		while ((c = getopt(argc, argv, "?ho:")) != -1) {
			switch(c) {
				case 'h':
				case '?': usage();
					  /* NOTREACHED */
				case 'o': OutputMixerFactory::construct(optarg, &output, &mixer);
					  break;
			}
		}
		argc -= optind;
		argv += optind;
		if (argc < 1) {
			fprintf(stderr, "error: no media path given\n");
			usage();
		}
		if (output == NULL) {
			fprintf(stderr, "error: no output plugin given\n");
			usage();
		}

		signal(SIGALRM, handle_update);
		signal(SIGWINCH, handle_resize);

		FolderFactory::construct(argv[0], &folder);
		interface = new Interface(output, mixer, folder);
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
