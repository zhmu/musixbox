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
#include "decode_mp3.h"
#include "decode_ogg.h"
#include "interface.h"
#include "interaction.h"

using namespace std;

char playbutton[10*10] = {
 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
 0, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
 0, 1, 1, 1, 1, 1, 0, 0, 0, 0,
 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
 0, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

char pausebutton[10*10] = {
 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
};

char stopbutton[10*10] = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

char filebutton[10*10] = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
 0, 1, 0, 1, 0, 0, 0, 1, 0, 1,
 1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
};

int
Interface::init()
{
	output = new OutputAO();
	if (!output->init())
		return 0;

	currentPath = "/geluid";
	playFile("mp3/09 - Dark Air.mp3");
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
			        	playFile(file); state++;
			        }
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
	vector<string> dirlist;
	bool dirty = true;
	bool rehash = true;
	int first_index;

	while (!interaction->mustTerminate()) {
		if (rehash) {
			dir = opendir(currentPath.c_str());
			if (dir == NULL)
				return string("");

			/*
			 * Read all directory items and place them in a vector, which
			 * we will sort later.
			 */
			dirlist.clear();
			while((dent = readdir(dir)) != NULL) {
				if (!strcmp(dent->d_name, "."))
					continue;
				dirlist.push_back(dent->d_name);
			}
			closedir(dir);
			sort(dirlist.begin(), dirlist.end());
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
			while (last_index < dirlist.size()) {
				if (y + interaction->getTextHeight() > interaction->getHeight())
					break;

				interaction->puttext(0, y, dirlist[last_index].c_str());
				y += interaction->getTextHeight();
				last_index++;
			}

			/* Draw a line-thing where someday the ^ and v buttons will be */
			for (int i = 0; i < interaction->getHeight(); i++) {
				interaction->putpixel(interaction->getWidth() - INTERFACE_BROWSER_BAR_SIZE, i, 1);
			}
			dirty = false;
		}

		/* See if there is any interaction */
		int x, y;
		if (!interaction->getCoordinates(&x, &y))
			continue;

		/* Left bar click? */
		if (x > interaction->getWidth() - INTERFACE_BROWSER_BAR_SIZE) {
			if (y > interaction->getHeight() / 2) {
				first_index = (first_index + 1) % dirlist.size();
			} else {
				first_index = (first_index - 1) % dirlist.size();
			}
			dirty = 1;
		} else {
			/* Item click! */
			int num = y / interaction->getTextHeight() + first_index;
			if (num >= 0 && num < dirlist.size()) {
				if (dirlist[num] == "..") {
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
				string path = currentPath + string("/") + dirlist[num];
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
				if (isPlayerPaused)
					cont();
				else
					pause();
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

static void*
player_wrapper(void* data)
{
	Interface* interface = (Interface*)data;

	interface->runDecoder();
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
		decoder->init();
		info = new InfoOgg(decoder);
	} else {
		/* assume MP3 */
		decoder = new DecoderMP3(input, output, visualizer);
		decoder->init();
		info = new InfoMP3(decoder);
	}
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

	hasPlayerThread = false;
}

void
Interface::blitImage(int x, int y, char* img)
{
	int i, j;

	for (j = 0; j < 10; j++) {
		for (i = 0; i < 10; i++) {
			interaction->putpixel(i + x, j + y, img[j*10+i]);
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
