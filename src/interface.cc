#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <string>
#include "output_ao.h"
#include "output_null.h"
#include "input_file.h"
#include "info_mp3.h"
#include "decode_mp3.h"
#include "decode_ogg.h"
#include "interface.h"
#include "interaction.h"

using namespace std;

int
Interface::init()
{
	output = new OutputNull();
	if (!output->init())
		return 0;

	currentPath = "/geluid";
	playFile("mp3/music.mp3");
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
	int state = 1;
	string file;

	while (!interaction->mustTerminate()) {
		switch(state) {
			case 0: file = launchBrowser();
			        if (file != "") {
			        	playFile(file); state++;
			        }
			        break;
			case 1: launchPlayer();
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

void
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
			s = "Unknown Artist";
			if (info != NULL && info->getArtist() != NULL) s = info->getArtist();
			interaction->puttext(2, 0, s);
			s = "Unknown Album";
			if (info != NULL && info->getAlbum() != NULL) s = info->getAlbum();
			interaction->puttext(2, interaction->getTextHeight(), s);
			s = "Unknown Title";
			if (info != NULL && info->getTitle() != NULL) s = info->getTitle();
			interaction->puttext(2, interaction->getTextHeight() * 2, s);
			dirty = false;
		}

		if (playingtime != oldplayingtime || totaltime != oldtotaltime) {
			interaction->clear(2, interaction->getTextHeight() * 3, interaction->getTextHeight(), interaction->getWidth());
			sprintf(temp, "%u:%02u / %u:%02u", playingtime / 60, playingtime % 60, totaltime / 60, totaltime % 60);
			interaction->puttext(2, interaction->getTextHeight() * 3, temp);
			oldplayingtime = playingtime; oldtotaltime = totaltime;
		}
	}
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
	InputFile* i = new InputFile();
	if (!i->open(fname.c_str()))
		return;
	input = i;

	string extension = string(fname.begin() + fname.find_last_of(".") + 1, fname.end());
	if (!strcasecmp(extension.c_str(), "ogg")) {
		decoder = new DecoderOgg();
		info = NULL;
	} else {
		/* assume MP3 */
		info = new InfoMP3();
		info->load(fname.c_str());
		decoder = new DecoderMP3();
	}
        decoder->setInput(input);

        decoder->setOutput(output);

	pthread_create(&player_thread, NULL, player_wrapper, this);
	hasPlayerThread = true;
}

void
Interface::stop()
{
	if (!hasPlayerThread)
		return;

	/* Ask the decoder thread to terminate, and wait until it is gone */
	decoder->terminate();
	pthread_join(player_thread, NULL);

	hasPlayerThread = false;
}
