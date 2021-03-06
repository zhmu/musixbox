#include <sys/types.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <termios.h>
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
	struct termios ti_orig, ti;

	/*
	 * For stdin, ignore signals, disable echoing and processing; this
	 * allows us to catch single charachters as well as ^C without
	 * having to resort to signals.
	 */
	tcgetattr(STDIN_FILENO, &ti_orig);
	memcpy(&ti, &ti_orig, sizeof(ti));
	ti.c_lflag &= ~(ECHO | ICANON | ISIG);
	ti.c_cflag &= CSIZE;
	ti.c_cflag |= CS8;
	tcsetattr(STDIN_FILENO, TCSANOW, &ti);

	for (int n = 0; n < argc; n++) {
		struct timeval tv;
		fd_set fds;

		/*
		 * Initialize playback. We wait half a second afterwards as we
		 * are waiting for the track information to become available -
		 * if the user hits ^C during this time, we quit immediately.
		 * Otherwise, we just skip the track and continue.
		 */
		printf("Playing %s...\n", argv[n]);
		player = new PlayPlayer(argv[n], output);
		player->play();
		tv.tv_sec = 0; tv.tv_usec = 500000;
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
		if (FD_ISSET(STDIN_FILENO, &fds)) {
			unsigned char ch;
			if (read(STDIN_FILENO, &ch, 1)) {
				if (ch == 3 /* ctrl-c */)
					break;
			}
		}

		/*
		 * Attempt to display the track information.
		 */
		Info* info = player->getInfo();
		printf("Artist: %s\n",
		 info != NULL && info->getArtist() != NULL ? info->getArtist() :
		 "(unknown)");
		printf("Album:  %s\n",
		 info != NULL && info->getAlbum() != NULL ? info->getAlbum() :
		 "(unknown)");
		printf("Title:  %s\n",
		 info != NULL && info->getTitle() != NULL ? info->getTitle() :
		 "(unknown)");
		if (info != NULL && info->getYear() != 0)
			printf("Year:   %u\n", info->getYear());
		else 
			printf("Year:   (unknown)\n");
		if (info != NULL && info->getNr() != 0)
			printf("Track#: %u\n", info->getNr());
		else 
			printf("Track#: (unknown)\n");

		/*
		 * Keep playing until either the track is done, or we are
		 * interrupted.
		 */
		while (!player->isFinished()) {
			unsigned int playingTime = player->getPlayingTime();
			unsigned int totalTime = player->getTotalTime();
			printf("\r%u:%02u / %u:%02u%s", playingTime / 60, playingTime % 60, totalTime / 60, totalTime % 60, player->isPaused() ? " - paused" : "");
			fflush(stdout);

			/*
			 * Wait for stdin input to arrive, or wait until a
			 * second elapsed; we need to perform work either
			 * way.
			 */
			struct timeval tv;
			fd_set fds;
			tv.tv_sec = 1; tv.tv_usec = 0;
			FD_ZERO(&fds);
			FD_SET(STDIN_FILENO, &fds);
			select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
			if (!FD_ISSET(STDIN_FILENO, &fds))
				/* Timeout was reached - update screen */
				continue;

			unsigned char ch;
			if (!read(STDIN_FILENO, &ch, 1))
				continue;

			if (ch == ' ' /* space */) {
				player->isPaused() ? player->cont() : player->pause();
				/* nuke any potential paused message */
				printf("\r                                             ");
			}
			if (ch == 3 /* ctrl-c */)
				break;

		}
		delete player; player = NULL;

		/* Print an extra newline to skip the timestamp above */
		printf("\n");
	}

	/* Restore the terminal to whatever it was */
	tcsetattr(STDIN_FILENO, TCSANOW, &ti_orig);
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
