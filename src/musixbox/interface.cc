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
#include "formAlphaBrowser.h"
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
	currentFolderChar = '\0';

	fBrowser = new formBrowser(interaction, folder);
	fPlayer = new formPlayer(interaction, this);
	fAlphaBrowser = new formAlphaBrowser(interaction);
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
	int state = 2;

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
				fBrowser->setFilterChar(fAlphaBrowser->getSelectedChar());
				fBrowser->run();
				switch (fBrowser->getReturnCode()) {
					case FORMBROWSER_CODE_SELECTED:
						currentFile = fBrowser->getSelectedFile();
						playFile();
						state = 1;
						break;
					case FORMBROWSER_CODE_CANCELED:
						state = 1;
						break;
					case FORMBROWSER_CODE_GOUP:
						state = 2;
						break;
				}
				break;
			case 1: /* player form */
				fPlayer->run();
				state = (fAlphaBrowser->getSelectedChar() == '\0') ? 2 : 0;
				break;
			case 2: /* alpha browser run */
				fAlphaBrowser->run();
				/*
				 * If something was selected, go to player - otherwise, go to the
				 * browser.
				 */
				state = (fAlphaBrowser->getSelectedChar() == '\0') ? 1 : 0;
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
	if (!fBrowser->getNextFile(currentFile))
		return;

	playFile();
}

void
Interface::prev() {
	if (!fBrowser->getPreviousFile(currentFile))
		return;

	playFile();
}

void
Interface::trackDone()
{
	/* The current track finished - let's try the next one */
	next();
}
