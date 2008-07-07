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


//! \brief Provides user interaction
class Interface {
friend	void* player_wrapper(void*);

public:
	/*! \brief Constructs a new interface object
	 *  \param i Interaction object to use
	 *  \param o Output object to use
	 *  \param path Root path of all media files
	 */
	Interface(Interaction* i, Output* o, const char* path) {
		interaction = i; output = o;
		input = NULL; decoder = NULL; visualizer = NULL; info = NULL;
		hasPlayerThread = false; player_thread = NULL; currentFile = "";
		hasTrackChanged = false; rootPath = std::string(path);
		scrollingEnabled = false;
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
	/*! \brief Update display for file/directory browser
	 *  \returns File to play, or empty string on error/cancel
	 */
	std::string launchBrowser();

	/*! \brief Launch the player
	 *  \return The new state of the user interface
	 */
	int launchPlayer();

	//! \brief Stops playing
	void stop();

	/*! \brief Plays a file
	 *  \param fname File to play
	 */
	void playFile(std::string fname);

	//! \brief Retrieve decoder object
	inline Decoder* getDecoder() { return decoder; }

	//! \brief Called if the decoder has finished decoding
	void signalDecoderFinished();

	//! \brief Relinquish control to the operating system
	void relinquish();

private:
	//! \brief Pause the player
	void pause();

	//! \brief Continue playing
	void cont();

	void blitImage(int x, int y, char* img);

	Interaction* interaction;

	//! \brief Root path where all media resides
	std::string rootPath;

	//! \brief Current path of the browser
	std::string currentPath;

	//! \brief Path where the currently playing item lives
	std::string playingPath;

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
	bool hasPlayerThread;

	//! \brief Is the playing thread paused?
	bool isPlayerPaused;

	//! \brief Has the playing track changed?
	bool hasTrackChanged;

	//! \brief Current file playing
	std::string currentFile;

	//! \brief Playing thread
	pthread_t player_thread;

	//! \brief Entries in the current directory
	std::vector<std::string> direntries;

	//! \brief Index to the entry we are playing
	unsigned int direntry_index;

	//! \brief Is scrolling of text in the interface enabled
	bool scrollingEnabled;
};

#endif /* __INTERFACE_H__ */
