#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>
#include <pthread_np.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <string>
#include "core/output.h"
#include "core/decoderfactory.h"
#include "core/exceptions.h"
#include "interface.h"
#include "interaction.h"
#include "formBrowser.h"
#include "formPlayer.h"

using namespace std;

Interface::Interface(Interaction* i, Output* o, Mixer* m, Folder* f, const char* resource)
{
	interaction = i; output = o; mixer = m; folder = f;
	input = NULL; decoder = NULL; visualizer = NULL; info = NULL;
	havePlayerThread = false; player_thread = NULL;
	if (resource != NULL)
		currentFile = std::string(resource);
	else
		currentFile = "";

	browser = new formBrowser(interaction, folder);
}

Interface::~Interface() {
	stop();
}

void
Interface::run()
{
	int state = 0;

	/* If a current file was passed, play it immediately */
	if (currentFile != "") {
		try {
			playFile();
			state = 1;
		} catch (MusixBoxException& e) {
			fprintf(stderr, "musixbox: unable to play initial file: %s\n", e.what());
		}
	}

	while (!interaction->mustTerminate()) {
		switch(state) {
			case 0: /* browser form */
		                browser->run();
		                if (browser->getSelectedFile() != "") {
	                                currentFile = browser->getSelectedFile();
			        	playFile();
			        }
				state = 1;
			        break;
			case 1: /* player form */
				formPlayer fp(interaction, this);
				fp.run();
				state = 0;
			        break;
		}
	}
}

void*
player_wrapper(void* data)
{
	Interface* interface = (Interface*)data;

	interface->getDecoder()->run();
	if (!interface->getDecoder()->isTerminating()) {
		/*
		 * Decoder was not forcefully terminated - play next track
		 */
		interface->next();
	}
	return NULL;
}

void
Interface::playFile()
{
	stop();

	if (currentFile == "")
		return;

	DecoderFactory::construct(currentFile, output, visualizer, &input, &decoder, &info);

	pthread_create(&player_thread, NULL, player_wrapper, this);
	havePlayerThread = true; playerPaused = false;
}

void
Interface::stop()
{
	if (!havePlayerThread)
		return;

	/* Ask the decoder thread to terminate, and wait until it is gone */
	decoder->terminate();
	cont();
	pthread_join(player_thread, NULL);

	Input* oldInput = input; Info* oldInfo = info; Decoder* oldDecoder = decoder;

	input = NULL; info = NULL; decoder = NULL;

	delete oldInput;
	delete oldInfo;
	delete oldDecoder;

	havePlayerThread = false;
}

void
Interface::pause()
{
	if (playerPaused || !havePlayerThread)
		return;

	pthread_suspend_np(player_thread);
	playerPaused = true;
}

void
Interface::cont()
{
	if (!playerPaused || !havePlayerThread)
		return;

	pthread_resume_np(player_thread);
	playerPaused = false;
}

void
Interface::next()
{
	/*
	 * If anything, ensure the decoder is gone - otherwise, the object
	 * will just linger on... */
	stop();

	if (!browser->getNextFile(currentFile))
		return;

	playFile();
}

void
Interface::prev() {
	if (!browser->getPreviousFile(currentFile))
		return;

	playFile();
}

void
Interface::relinquish()
{
	/*
	 * This is used to prevent us from hogging the CPU 100% waiting for
	 * events that are being handeled so fast no one can notice anyway :)
	 */
	//usleep(100);
}
