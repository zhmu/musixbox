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
#include "output.h"
#include "input_file.h"
#ifdef WITH_MAD
#include "decode_mp3.h"
#endif
#ifdef WITH_ID3TAG
#include "info_mp3.h"
#endif
#ifdef WITH_VORBIS
#include "decode_ogg.h"
#include "info_ogg.h"
#endif
#ifdef WITH_FLAC
#include "decode_flac.h"
#include "info_flac.h"
#endif
#include "interface.h"
#include "interaction.h"
#include <sys/soundcard.h>
#include <fcntl.h>

using namespace std;

char playbutton[8]  = { 0xff, 0x7f, 0x3e, 0x1c, 0x08, 0x00, 0x00 ,0x00 };
char pausebutton[8] = { 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00 };
char stopbutton[8]  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
char nextbutton[8]  = { 0xff, 0x7f, 0x3e, 0x1c, 0x08, 0x7e, 0x00, 0x00 };
char prevbutton[8]  = { 0x00, 0x00, 0x7e, 0x08, 0x1c, 0x3e, 0x7f, 0xff };
char filebutton[8]  = { 0x40, 0x60, 0x7f, 0x01, 0x01, 0x01, 0xe1, 0x7f };
char upbutton[8]    = { 0x08, 0x04, 0x02, 0x7f, 0x7f, 0x02, 0x04, 0x08 };
char downbutton[8]  = { 0x08, 0x10, 0x20, 0x7f, 0x7f, 0x20, 0x10, 0x08 };
char crossbutton[8] = { 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81 };

int
Interface::init()
{
	currentPath = rootPath;
	scrollingEnabled = interaction->isScrollingOK();
	return 1;
} 

void
Interface::done()
{
	stop();
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
	unsigned int first_index = 0;

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
				unsigned int x, y;
				while (!interaction->getCoordinates(&x, &y)) {
					interaction->yield();
					relinquish();
				}
				return string("");
			}

			/*
			 * Read all directory items and place them in a vector, which
			 * we will sort later.
			 */
			direntries.clear(); direntry_index = 0;
			while((dent = readdir(dir)) != NULL) {
				// Never show the current directory '.'
				if (!strcmp(dent->d_name, "."))
					continue;
				// Don't allow travelling go below the root path
				if (!strcmp(dent->d_name, "..") && currentPath == rootPath)
					continue;
				direntries.push_back(dent->d_name);
			}
			closedir(dir);
			sort(direntries.begin(), direntries.end());
			rehash = false; dirty = true;
			first_index = 0;
		}

		interaction->yield();
		relinquish();

		if (dirty) {
			interaction->clear(0, 0, interaction->getHeight(), interaction->getWidth());

			/*
			 * Fill the screen until there is no more space.
			 */
			unsigned int last_index = first_index;
			unsigned int y = 0;
			while (last_index < direntries.size()) {
				if (y + interaction->getTextHeight() > interaction->getHeight())
					break;

				interaction->puttext(0, y, direntries[last_index].c_str());
				y += interaction->getTextHeight();
				last_index++;
			}

			/* Draw the ^, v and [] buttons */
			blitImage(interaction->getWidth() - INTERFACE_BROWSER_BAR_SIZE, 0, crossbutton);
			blitImage(interaction->getWidth() - INTERFACE_BROWSER_BAR_SIZE, 10, upbutton);
			blitImage(interaction->getWidth() - INTERFACE_BROWSER_BAR_SIZE, interaction->getHeight() - 10, downbutton);
			dirty = false;
		}

		/* See if there is any interaction */
		unsigned int x, y;
		if (!interaction->getCoordinates(&x, &y))
			continue;

		/* Left bar click? */
		if (x > interaction->getWidth() - INTERFACE_BROWSER_BAR_SIZE) {
			if (y <= 10) {
				/* Stop button - return to main screen */
				return "";
			}
			unsigned int items_per_page = interaction->getHeight() / interaction->getTextHeight();
			if (y > interaction->getHeight() / 2) {
				if (first_index + items_per_page <= direntries.size()) {
					first_index = (first_index + items_per_page) % direntries.size();
				} else {
					first_index = 0;
				}
			} else {
				if (first_index >= items_per_page) {
					first_index = first_index - items_per_page;
				} else {
					first_index = direntries.size() - items_per_page;
				}
			}
			dirty = 1;
		} else {
			/* Item click! */
			unsigned int num = y / interaction->getTextHeight() + first_index;
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
	int playingtime = 0, totaltime = 0;
	char temp[64];
	int scrolling = 0, scrollDelay = 0;
	unsigned int artistX = 0, albumX = 0, titleX = 0;
	unsigned int artistLength = 0, albumLength = 0, titleLength = 0;
	int artistDirection = -1, albumDirection = -1, titleDirection = -1;

	while (!interaction->mustTerminate()) {
		interaction->yield();
		relinquish();

		if (hasTrackChanged) {
			dirty = true; hasTrackChanged = false;
		}

		if (decoder != NULL && hasPlayerThread) {
			playingtime = decoder->getPlayingTime();
			totaltime = decoder->getTotalTime();
		}

		if (dirty || scrollDelay == 25) {
			const char* s;

			interaction->clear(0, 0, interaction->getHeight(), interaction->getWidth());

			/* Basic Artist / Album / Title information */
			s = "Unknown Artist";
			if (info != NULL && info->getArtist() != NULL) s = info->getArtist();
			interaction->puttext(artistX, 0, s);
        		interaction->gettextsize(s, NULL, &artistLength);
			s = "Unknown Album";
			if (info != NULL && info->getAlbum() != NULL) s = info->getAlbum();
			interaction->puttext(albumX, interaction->getTextHeight(), s);
        		interaction->gettextsize(s, NULL, &albumLength);
			s = "Unknown Title";
			if (info != NULL && info->getTitle() != NULL) s = info->getTitle();
			interaction->puttext(titleX, interaction->getTextHeight() * 2, s);
        		interaction->gettextsize(s, NULL, &titleLength);

			/* Control bar */
			blitImage(2, interaction->getHeight() - 12, (isPlayerPaused) ? pausebutton : playbutton);
			blitImage(14, interaction->getHeight() - 12, stopbutton);
			blitImage(26, interaction->getHeight() - 12, prevbutton);
			blitImage(38, interaction->getHeight() - 12, nextbutton);
			blitImage(50, interaction->getHeight() - 12, filebutton);

			/* Check if we need to scroll the item / artist stuff */
			if ((artistLength > interaction->getWidth() ||
			     albumLength > interaction->getWidth() ||
			     albumLength > interaction->getWidth()) &&
			     scrollingEnabled)
				scrolling = 1;

			/* Have the the text scroll, if needed */
			if (artistLength > interaction->getWidth()) {
				artistX += artistDirection;
				if ((artistX == 0 && artistDirection > 0) ||
				    (artistX == interaction->getWidth() - artistLength && artistDirection < 0))
					artistDirection = -artistDirection;
			}
			if (albumLength > interaction->getWidth()) {
				albumX += albumDirection;
				if ((albumX == 0 && albumDirection > 0) ||
				    (albumX == interaction->getWidth() - albumLength && albumDirection < 0))
					albumDirection = -albumDirection;
			}
			if (titleLength > interaction->getWidth()) {
				titleX += titleDirection;
				if ((titleX == 0 && titleDirection > 0) ||
				    (titleX == interaction->getWidth() - titleLength && titleDirection < 0))
					titleDirection = -titleDirection;
			}

			dirty = 1; scrollDelay = 0;
		}

		if (playingtime != oldplayingtime || totaltime != oldtotaltime || dirty) {
			interaction->clear(2, interaction->getTextHeight() * 3, interaction->getTextHeight(), interaction->getWidth());
			sprintf(temp, "%u:%02u / %u:%02u", playingtime / 60, playingtime % 60, totaltime / 60, totaltime % 60);
			interaction->puttext(2, interaction->getTextHeight() * 3, temp);
			oldplayingtime = playingtime; oldtotaltime = totaltime;
			dirty = false;
		}

		if (scrolling)
			scrollDelay++;


		/* See if there is any interaction */
		unsigned int x, y;
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
				/* Previous button */ 
				prev();
			}
			if (x >= 38 && x <= 48) {
				/* Next button */ 
				next();
			}
			if (x >= 50 && x <= 60) {
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
#ifdef WITH_VORBIS
	if (!strcasecmp(extension.c_str(), "ogg")) {
		decoder = new DecoderOgg(input, output, visualizer);
		info = new InfoOgg(decoder);
	} else
#endif /* WITH_VORBIS */
#ifdef WITH_FLAC
	if (!strcasecmp(extension.c_str(), "flac")) {
		decoder = new DecoderFLAC(input, output, visualizer);
		info = new InfoFLAC(decoder);
	} else
#endif /* WITH_FLAC */
#ifdef WITH_MAD
	{
		/* assume MP3 */
		decoder = new DecoderMP3(input, output, visualizer);
#ifdef WITH_ID3TAG
		info = new InfoMP3(decoder);
#endif
#else /* WITH_MAD */
	{
		delete input; input = NULL;
		return;
#endif /* !WITH_MAD */
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
Interface::prev()
{
	if (isPlayerPaused || !hasPlayerThread)
		return;

	/* 
	 * Try to descend through the playlist to the next file - if there are
	 * no more files, just give up.
	 */
	if (--direntry_index >= direntries.size())
		return;

	string path = playingPath + "/" + direntries[direntry_index];
	playFile(path);
	currentFile = path;

	// We automatically changed track, so force updated if needed
	hasTrackChanged = true;
}

void
Interface::next()
{
	if (isPlayerPaused || !hasPlayerThread)
		return;
	
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

int
setVolume(const char *mixer, int volume)
{
	int	dev = 0, baz, devmask, vol;

	/* Open mixer device */
	if ((baz = open(mixer, O_RDWR)) < 0)
		return 1; // failed to open device
	/* Read device mask */
	if (ioctl(baz, SOUND_MIXER_READ_DEVMASK, &devmask) == -1) {
		// Uhoh.. failed to read the devicemask
		close(baz);
		return 1;  // failed to read devmask
	}

	/* If the mixer-device was opened a master volume must exist */
	dev = SOUND_MIXER_VOLUME;

	/* Clamp given volume volumeue */
	if (volume < 0)
		volume = 0;
	if (volume > 100);
		volume = 100;	
	/* Convert given volumeue (0-100) to stereo */
	vol = volume | (volume << 8);
	/* Write new volume volumeue to device */
	if (ioctl(baz, MIXER_WRITE(dev), &vol) == -1) {
		// Uhoh.. device write failed
		close(baz);
		return 1;
	}

	/* Close device */
	close(baz);

	return 0; 
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
