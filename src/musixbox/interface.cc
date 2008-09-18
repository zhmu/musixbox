#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <string>
#include "core/output.h"
#include "core/decoderfactory.h"
#include "core/exceptions.h"
#include "ui/interaction.h"
#include "interface.h"
#include "formBrowser.h"
#include "formPlayer.h"

using namespace std;

Interface::Interface(Interaction* i, Output* o, Mixer* m, Folder* f, const char* resource)
{
	interaction = i; output = o; mixer = m; folder = f; visualizer = NULL; player = NULL;
	if (resource != NULL)
		currentFile = std::string(resource);
	else
		currentFile = "";

	browser = new formBrowser(interaction, folder);
}

Interface::~Interface() {
	if (player != NULL) {
		player->stop();
		delete player;
	}
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

void
Interface::playFile()
{
	if (player != NULL) {
		player->stop();
		delete player;
		player = NULL;
	}

	if (currentFile == "")
		return;

	player = new BoxPlayer(currentFile, output, visualizer, this);
	player->play();
}

void
Interface::next()
{
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
Interface::trackDone()
{
	/* The current track finished - let's try the next one */
	next();
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
