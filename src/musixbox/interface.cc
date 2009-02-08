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
#include "formPlaylist.h"

using namespace std;

Interface::Interface(Interaction* i, Output* o, Mixer* m, Folder* f, const char* resource)
{
	interaction = i; output = o; mixer = m; folder = f; visualizer = NULL; player = NULL;
	if (resource != NULL)
		currentFile = std::string(resource);
	else
		currentFile = "";
	currentFolderChar = '\0';

	fBrowser = new formBrowser(interaction, this, folder);
	fPlayer = new formPlayer(interaction, this);
	fAlphaBrowser = new formAlphaBrowser(interaction, folder);
	fPlaylist = new formPlaylist(interaction, this);
	playingFromPlaylist = false;
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
			playResource(currentFile);
			state = 1;
		} catch (MusixBoxException& e) {
			fprintf(stderr, "musixbox: unable to play initial file: %s\n", e.what());
		}
	}

	while (!interaction->mustTerminate()) {
		switch(state) {
			case 0: /* browser form */
				fBrowser->setFilterChar(fAlphaBrowser->getSelectedChar());
				switch (fBrowser->run()) {
					case FORMBROWSER_CODE_SELECTED:
						currentFile = fBrowser->getSelectedFile();
						playResource(currentFile);
						state = 1;
						break;
					case FORMBROWSER_CODE_CANCELED:
						state = 1;
						break;
					case FORMBROWSER_CODE_GOUP:
						state = 2;
						break;
					case FORMBROWSER_CODE_QUEUED:
						state = 1;
						break;
				}
				break;
			case 1: /* player form */
				switch (fPlayer->run()) {
					case 0:
						state = (fAlphaBrowser->getSelectedChar() == '\0') ? 2 : 0;
						break;
					case 1:
						state = 3;
						break;
				}
				break;
			case 2: /* alpha browser run */
				fAlphaBrowser->run();
				/*
				 * If something was selected, go to player - otherwise, go to the
				 * browser.
				 */
				state = (fAlphaBrowser->getSelectedChar() == '\0') ? 1 : 0;
				break;
			case 3: /* playlist form */
				fPlaylist->run();
				state = 1;
				break;
		}
	}
}

void
Interface::playResource(string resource)
{
	if (player != NULL) {
		player->stop();
		delete player;
		player = NULL;
	}

	if (resource == "")
		return;

	player = new BoxPlayer(resource, output, visualizer, this);
	player->play();
}

void
Interface::next()
{
	if (!playingFromPlaylist)
		return;

	/*
	 * We use the playlist like a stack: we pop off the first
	 * item and play it. This ensures the screen will not be
	 * littered with playlist items we have already played.
	 */
	playlist.removeItem(0);
	playlist.setCurrentPlayItem(0);
	string resource = playlist.getCurrentResource();
	if (resource == "") {
		/* End of playlist */
		playingFromPlaylist = false;
		return;
	}

	playResource(resource);
}

void
Interface::trackDone()
{
	/* The current track finished - let's try the next one */
	next();
}

void
Interface::addToPlaylist(string resource)
{
	if (resource == "." || resource == "..")
		return;

	/* If the resource is a folder, enter it and recursively add */
	if (folder->isFolder(resource)) {
		folder->select(resource);
		for (unsigned int i = 0; i < folder->getEntries().size(); i++)
			addToPlaylist(folder->getEntries()[i]);
		folder->goUp();
		return;
	}

	/* Upon adding, resolve the complete path */
	try {
		playlist.addItem(new PlaylistItem(folder->getFullPath(resource)));
	} catch (DecoderException& e) {
		/* Something made the decoder unhappy - just skip the file */
	}
}

void
Interface::startPlaylist(int num) {
	playlist.setCurrentPlayItem(num);
	string resource = playlist.getCurrentResource();
	if (resource == "")
		return;

	playResource(resource);
	playingFromPlaylist = true;
}
