#include <sys/types.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <getopt.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include "core/outputmixerfactory.h"
#include "core/exceptions.h"
#include "core/player.h"
#include "misc/configuration.h"

using namespace std;

#define DEFAULT_CONFIG_FILE ".musixboxrc"

class PlayPlayer : public Player {
public:
	PlayPlayer(std::string resource, Output* o)
		: Player(resource, o, NULL)
	{
		finished = false;
	}

	inline bool isFinished() {
		return finished;
	}

	inline void terminated() {
		finished = true;
	}

private:
	bool finished;
};

Configuration* config;
PlayPlayer* player = NULL;
int quit = 0;

void
sigint(int num)
{
	quit++;
}

void
usage()
{
	fprintf(stderr, "usage: musixplay [-?hn] [-o type] [-p path] file ...\n\n");
	fprintf(stderr, " -h, -?         this help\n");
	fprintf(stderr, " -o type        select output plugin\n");
	fprintf(stderr, "                available are:");
	list<string> l;
	OutputMixerFactory::getAvailable(l);
	for (list<string>::iterator it = l.begin(); it != l.end(); it++) {
		fprintf(stderr, " %s", (*it).c_str());
	}
	fprintf(stderr, "\n");
	fprintf(stderr, " -c config      location of configuration file\n");
	fprintf(stderr, "                default: ~/%s\n", DEFAULT_CONFIG_FILE);
	fprintf(stderr, " -n             clear configuration file\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "file ... is the list of files to play\n");
	exit(EXIT_SUCCESS);
}

void
play(int argc, char** argv, Output* output)
{
	for (int n = 0; n < argc; n++) {
		if (quit)
			break;
		printf("Playing %s...\n", argv[n]);
		player = new PlayPlayer(argv[n], output);
		player->play();
		while (!player->isFinished() && !quit) {
			sleep(1);
		}
		delete player; player = NULL;
	}
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
		Mixer* mixer;
		int c;

		while ((c = getopt(argc, argv, "?hnc:l:o:p:")) != -1) {
			switch(c) {
				case 'h':
				case '?': usage();
					  /* NOTREACHED */
				case 'o': OutputMixerFactory::construct(optarg, &output, &mixer);
					  /* If we got here, this worked - so store the provider */
					  config->setString("output provider", optarg);
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
		if (argc == 0) {
			fprintf(stderr, "error: no files to play\n");
			return EXIT_FAILURE;
		}

		if (output == NULL) {
			string str = config->getString("output provider", "");
			if (str == "") {
				fprintf(stderr, "fatal: no output provider, aborting\n");
				return EXIT_FAILURE;
			}
			OutputMixerFactory::construct(str, &output, &mixer);
		}

		signal(SIGINT, sigint);

		play(argc, argv, output);

		/* Only store the configuration on successful termination */
		config->store();

		if (player != NULL) {
			player->stop();
			delete player;
		}
		delete config;
		delete output;
	} catch (MusixBoxException& e) {
		fprintf(stderr, "%s\n", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
