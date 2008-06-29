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
#include "input_file.h"
#include "decode_mp3.h"
#include "decode_ogg.h"
#include "interface.h"
#include "interaction.h"

using namespace std;

int
Interface::init()
{
	output = new OutputAO();
	if (!output->init())
		return 0;

	currentPath = "/geluid";

	play(string("mp3/music.mp3"));
	launchPlayer();
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
	while (!interaction->mustTerminate()) {
		interaction->yield();
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
					printf("==>[%s]\n", currentPath.c_str());
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
}

void
Interface::launchPlayer()
{
	bool dirty = true;

	while (!interaction->mustTerminate()) {
		interaction->yield();

		if (dirty) {
			interaction->clear(0, 0, interaction->getHeight(), interaction->getWidth());
			interaction->puttext(2, 0, "Artist");
			interaction->puttext(2, interaction->getTextHeight(), "Album");
			interaction->puttext(2, interaction->getTextHeight() * 2, "Title");
			interaction->puttext(2, interaction->getTextHeight() * 3, "0:00 / 0:00");
			dirty = false;
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
Interface::play(string fname)
{
	InputFile* i = new InputFile();
	if (!i->open(fname.c_str()))
		return;
	input = i;
	
	decoder = new DecoderMP3();
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
