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
#include "core/input_file.h"
#ifdef WITH_MAD
#include "core/decode_mp3.h"
#endif
#ifdef WITH_ID3TAG
#include "core/info_mp3.h"
#endif
#ifdef WITH_VORBIS
#include "core/decode_ogg.h"
#include "core/info_ogg.h"
#endif
#ifdef WITH_FLAC
#include "core/decode_flac.h"
#include "core/info_flac.h"
#endif
#ifdef WITH_MIKMOD
#include "core/decode_module.h"
#include "core/info_module.h"
#endif
#ifdef WITH_SIDPLAY2
#include "core/decode_sid.h"
#include "core/info_sid.h"
#endif
#ifdef WITH_CURL
#include "core/input_remote.h"
#endif
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

#ifdef WITH_CURL
	/*
	 * If we find :// in the filename and CURL is available, assume we
	 * are playing a stream.
	 */
	if (currentFile.find("://") != std::string::npos) {
		input = new InputRemote(currentFile);
	} else
#endif
		input = new InputFile(currentFile);

	string extension = string(currentFile.begin() + currentFile.find_last_of(".") + 1, currentFile.end());
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
#ifdef WITH_MIKMOD
	if (!strcasecmp(extension.c_str(), "mod")) {
		decoder = new DecoderModule(input, output, visualizer);
		info = new InfoModule(decoder);
	} else
#endif /* WITH_MIKMOD */
#ifdef WITH_SIDPLAY2
	if (!strcasecmp(extension.c_str(), "sid")) {
		decoder = new DecoderSID(input, output, visualizer);
		info = new InfoSID(decoder);
	} else
#endif /* WITH_SIDPLAY2 */
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
		info->load(currentFile.c_str());

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
