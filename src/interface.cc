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
#include "output_ao.h"
#include "output_null.h"
#include "input_file.h"
#include "info_mp3.h"
#include "info_ogg.h"
#include "info_flac.h"
#include "decode_mp3.h"
#include "decode_ogg.h"
#include "decode_flac.h"
#include "interface.h"
#include "interaction.h"

using namespace std;

char playbutton[8]  = { 0xff, 0x7f, 0x3e, 0x1c, 0x08, 0x00, 0x00 ,0x00 };
char pausebutton[8] = { 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00 };
char stopbutton[8]  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
char filebutton[8]  = { 0x40, 0x60, 0x7f, 0x01, 0x01, 0x01, 0xe1, 0x7f };
char upbutton[8]    = { 0x08, 0x04, 0x02, 0x7f, 0x7f, 0x02, 0x04, 0x08 };
char downbutton[8]  = { 0x08, 0x10, 0x20, 0x7f, 0x7f, 0x20, 0x10, 0x08 };
char crossbutton[8] = { 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81 };

int
Interface::init()
{
	output = new OutputAO();
	if (!output->init())
		return 0;

	currentPath = "/geluid";
	return 1;
} 

void
Interface::done()
{
	stop();
	output->done();
}

void
Interface::run()
{
	int state = 0;
	string file;

	while (!interaction->mustTerminate()) {
		switch(state) {
			case 0: file = launchBrowser();
			        if (file != "") {
					playingPath = currentPath;
					currentFile = file;
			        	playFile(file);
			        }
				state = 1;
			        break;
			case 1: state = launchPlayer();
			        break;
		}
	}
}

string
Interface::launchBrowser()
{
	DIR* dir;
	struct dirent* dent;
	bool dirty = true;
	bool rehash = true;
	int first_index;

	while (!interaction->mustTerminate()) {
		if (rehash) {
			dir = opendir(currentPath.c_str());
			if (dir == NULL) {
				/*
				 * Can't open folder - report this nicely.
				 */
				interaction->clear(0, 0, interaction->getHeight(), interaction->getWidth());
				interaction->puttext(2, 0, "Unable to open folder");
				interaction->puttext(2, interaction->getTextHeight(), currentPath.c_str());
				interaction->puttext(2, interaction->getTextHeight() * 2, "<interact to continue>");
				int x, y;
				while (!interaction->getCoordinates(&x, &y)) {
					interaction->yield();
				}
				return string("");
			}

			/*
			 * Read all directory items and place them in a vector, which
			 * we will sort later.
			 */
			direntries.clear(); direntry_index = 0;
			while((dent = readdir(dir)) != NULL) {
				if (!strcmp(dent->d_name, "."))
					continue;
				direntries.push_back(dent->d_name);
			}
			closedir(dir);
			sort(direntries.begin(), direntries.end());
			rehash = false; dirty = true;
			first_index = 0;
		}

		interaction->yield();

		if (dirty) {
			interaction->clear(0, 0, interaction->getHeight(), interaction->getWidth());

			/*
			 * Fill the screen until there is no more space.
			 */
			int last_index = first_index;
			int y = 0;
			while (last_index < direntries.size()) {
				if (y + interaction->getTextHeight() > interaction->getHeight())
					break;

				interaction->puttext(0, y, direntries[last_index].c_str());
				y += interaction->getTextHeight();
				last_index++;
			}

			/* Draw the ^, v and [] buttons */
			blitImage(interaction->getWidth() - 10, 0, crossbutton);
			blitImage(interaction->getWidth() - 10, 10, upbutton);
			blitImage(interaction->getWidth() - 10, interaction->getHeight() - 10, downbutton);
			dirty = false;
		}

		/* See if there is any interaction */
		int x, y;
		if (!interaction->getCoordinates(&x, &y))
			continue;

		/* Left bar click? */
		if (x > interaction->getWidth() - INTERFACE_BROWSER_BAR_SIZE) {
			if (y <= 10) {
				/* Stop button - return to main screen */
				return "";
			}
			if (y > interaction->getHeight() / 2) {
				first_index = (first_index + (interaction->getHeight() / 10)) % direntries.size();
			} else {
				first_index = (first_index - (interaction->getHeight() / 10)) % direntries.size();
			}
			dirty = 1;
		} else {
			/* Item click! */
			int num = y / interaction->getTextHeight() + first_index;
			if (num >= 0 && num < direntries.size()) {
				if (direntries[num] == "..") {
					/* Need to go one level lower, so strip
					 * off the last /path item */
					currentPath = string(currentPath.begin(), currentPath.begin() + currentPath.find_last_of("/"));
					rehash = true;
					continue;
				}

				/*
				 * We have an item - construct full path to see
				 * if it's a file or not
				 */
				string path = currentPath + string("/") + direntries[num];
				struct stat fs;
				if (stat(path.c_str(), &fs) < 0)
					continue;
				if (S_ISDIR(fs.st_mode)) {
					/* It's a path, so enter it */
					currentPath = path;
					rehash = true;
					continue;
				} else {
					/* We got a file! */
					direntry_index = num;
					return path;
				}
			}
		}
	}

	return "";
}

int
Interface::launchPlayer()
{
	bool dirty = true;
	int oldplayingtime = -1, oldtotaltime = -1;
	int playingtime, totaltime;
	char temp[64];

	while (!interaction->mustTerminate()) {
		interaction->yield();

		if (hasTrackChanged) {
			dirty = true; hasTrackChanged = false;
		}

		if (decoder != NULL && hasPlayerThread) {
			playingtime = decoder->getPlayingTime();
			totaltime = decoder->getTotalTime();
		}

		if (dirty) {
			const char* s;

			interaction->clear(0, 0, interaction->getHeight(), interaction->getWidth());

			/* Basic Artist / Album / Title information */
			s = "Unknown Artist";
			if (info != NULL && info->getArtist() != NULL) s = info->getArtist();
			interaction->puttext(2, 0, s);
			s = "Unknown Album";
			if (info != NULL && info->getAlbum() != NULL) s = info->getAlbum();
			interaction->puttext(2, interaction->getTextHeight(), s);
			s = "Unknown Title";
			if (info != NULL && info->getTitle() != NULL) s = info->getTitle();
			interaction->puttext(2, interaction->getTextHeight() * 2, s);

			/* Control bar */
			blitImage(2, interaction->getHeight() - 12, (isPlayerPaused) ? pausebutton : playbutton);
			blitImage(14, interaction->getHeight() - 12, stopbutton);
			blitImage(26, interaction->getHeight() - 12, filebutton);
		}

		if (playingtime != oldplayingtime || totaltime != oldtotaltime || dirty) {
			interaction->clear(2, interaction->getTextHeight() * 3, interaction->getTextHeight(), interaction->getWidth());
			sprintf(temp, "%u:%02u / %u:%02u", playingtime / 60, playingtime % 60, totaltime / 60, totaltime % 60);
			interaction->puttext(2, interaction->getTextHeight() * 3, temp);
			oldplayingtime = playingtime; oldtotaltime = totaltime;
			dirty = false;
		}

		/* See if there is any interaction */
		int x, y;
		if (!interaction->getCoordinates(&x, &y))
			continue;

		if (y > interaction->getHeight() - 12) {
			/* We are at the bottom bar */
			if (x >= 2 && x <= 12) {
				/* Play/resume button */
				if (hasPlayerThread)  {
					if (isPlayerPaused)
						cont();
					else
						pause();
				} else if (currentFile != "") {
					playFile(currentFile);
				}
				dirty = true;
			}
			if (x >= 14 && x <= 24) {
				/* Stop button */
				stop();
			}
			if (x >= 26 && x <= 36) {
				/* File button - go to new state */
				return 0;
			}
		}
	}

	return 1;
}

void*
player_wrapper(void* data)
{
	Interface* interface = (Interface*)data;

	interface->getDecoder()->run();
	if (!interface->getDecoder()->isTerminating()) {
		/*
		 * Decoder was not forcefully terminated - signal the event
		 */
		interface->signalDecoderFinished();
	}
	return NULL;
}

void
Interface::playFile(string fname)
{
	stop();

	InputFile* i = new InputFile();
	if (!i->open(fname.c_str()))
		return;
	input = i;

	string extension = string(fname.begin() + fname.find_last_of(".") + 1, fname.end());
	if (!strcasecmp(extension.c_str(), "ogg")) {
		decoder = new DecoderOgg(input, output, visualizer);
		info = new InfoOgg(decoder);
	} else if (!strcasecmp(extension.c_str(), "flac")) {
		decoder = new DecoderFLAC(input, output, visualizer);
		info = new InfoFLAC(decoder);
	} else {
		/* assume MP3 */
		decoder = new DecoderMP3(input, output, visualizer);
		info = new InfoMP3(decoder);
	}
	if (info != NULL)
		info->load(fname.c_str());

	pthread_create(&player_thread, NULL, player_wrapper, this);
	hasPlayerThread = true; isPlayerPaused = false;
}

void
Interface::stop()
{
	if (!hasPlayerThread)
		return;

	/* Ask the decoder thread to terminate, and wait until it is gone */
	decoder->terminate();
	cont();
	pthread_join(player_thread, NULL);

	delete input;
	delete info;
	delete decoder;

	input = NULL; info = NULL; decoder = NULL;
	hasPlayerThread = false;
}

void
Interface::blitImage(int x, int y, char* img)
{
	int i, j;

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			interaction->putpixel(i + x, j + y, (img[i] & (1 << j)) ? 1 : 0);
		}
	}
}

void
Interface::pause()
{
	if (isPlayerPaused || !hasPlayerThread)
		return;

	pthread_suspend_np(player_thread);
	isPlayerPaused = true;
}

void
Interface::cont()
{
	if (!isPlayerPaused || !hasPlayerThread)
		return;

	pthread_resume_np(player_thread);
	isPlayerPaused = false;
}

void
Interface::signalDecoderFinished()
{
	/* 
	 * Try to ascend through the playlist to the next file - if there are
	 * no more files, just give up.
	 */
	if (++direntry_index >= direntries.size())
		return;

	string path = playingPath + "/" + direntries[direntry_index];
	playFile(path);
	currentFile = path;

	// We automatically changed track, so force updated if needed
	hasTrackChanged = true;
}
