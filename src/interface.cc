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
#ifdef WITH_MIKMOD
#include "decode_module.h"
#include "info_module.h"
#endif
#ifdef WITH_SIDPLAY2
#include "decode_sid.h"
#include "info_sid.h"
#endif
#include "interface.h"
#include "interaction.h"
#include "formBrowser.h"
#include "formPlayer.h"

using namespace std;

Interface::Interface(Interaction* i, Output* o, const char* path, Mixer* m) {
	interaction = i; output = o; mixer = m;
	input = NULL; decoder = NULL; visualizer = NULL; info = NULL;
	havePlayerThread = false; player_thread = NULL; currentFile = "";
	rootPath = std::string(path);

	browser = new formBrowser(interaction, rootPath);
}

Interface::~Interface() {
	stop();
}

void
Interface::run()
{
	int state = 0;

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
