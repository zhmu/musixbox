#include <pthread.h>
#include <string>
#include <vector>
#include "info.h"
#include "interaction.h"
#include "output.h"
#include "visualize.h"
#include "decode.h"

#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#define INTERFACE_BROWSER_BAR_SIZE	10

class formBrowser;

//! \brief Provides user interaction
class Interface {
friend	void* player_wrapper(void*);
friend  class formPlayer;

public:
	/*! \brief Constructs a new interface object
	 *  \param i Interaction object to use
	 *  \param o Output object to use
	 *  \param path Root path of all media files
	 */
	Interface(Interaction* i, Output* o, const char* path) {
		interaction = i; output = o;
		input = NULL; decoder = NULL; visualizer = NULL; info = NULL;
		havePlayerThread = false; player_thread = NULL; currentFile = "";
		rootPath = std::string(path);
	}

	/*! \brief Initialize interface provider
	 *  \return Non-zero on success
	 */
	int init();

	//! \brief Run the interface 
	void run();

	//! \brief Deinitialize interaction provider
	void done();

	//! \brief Start the decoder
	void runDecoder() { decoder->run(); }

protected:
	//! \brief Stops playing
	void stop();

	//! \brief Plays the current file
	void playFile();

	//! \brief Retrieve decoder object
	inline Decoder* getDecoder() { return decoder; }

	//! \brief Retrieve info object
	inline Info* getInfo() { return info; }

	//! \brief Play next track
	void next();

	//! \brief Play previus track
	void prev();

	//! \brief Relinquish control to the operating system
	void relinquish();

	//! \brief Is the player paused?
	bool isPlayerPaused() { return playerPaused; }

	//! \brief Is there a play thread around?
	bool isPlayerThread() { return havePlayerThread; }

private:
	//! \brief Pause the player
	void pause();

	//! \brief Continue playing
	void cont();

	//! \brief Interaction object we are using
	Interaction* interaction;

	//! \brief Root path where all media resides
	std::string rootPath;

	//! \brief Output object
	Output* output;

	//! \brief Input object
	Input* input;

	//! \brief Decoder object
	Decoder* decoder;

	//! \brief Visualization object
	Visualizer* visualizer;

	//! \brief Informatiom object
	Info* info;

	//! \brief Do we have a playing thread?
	bool havePlayerThread;

	//! \brief Is the playing thread paused?
	bool playerPaused;

	//! \brief Current file playing
	std::string currentFile;

	//! \brief Playing thread
	pthread_t player_thread;

	//! \brief Browser form
	formBrowser* browser;
};

#endif /* __INTERFACE_H__ */
