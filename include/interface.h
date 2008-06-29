#include <pthread.h>
#include <string>
#include "info.h"
#include "interaction.h"
#include "output.h"
#include "visualize.h"
#include "decode.h"

#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#define INTERFACE_BROWSER_BAR_SIZE	8

//! \brief Provides user interaction
class Interface {
public:
	//! \brief Constructs a new interface object
	Interface(Interaction* i) {
		interaction = i;
		output = NULL; input = NULL; decoder = NULL; visualizer = NULL; info = NULL;
		hasPlayerThread = false; player_thread = NULL;
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

private:
	//! \brief Pause the player
	void pause();

	//! \brief Continue playing
	void cont();

	void blitImage(int x, int y, char* img);

	Interaction* interaction;

	//! \brief Current path of the browser
	std::string currentPath;

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

	//! \brief Playing thread
	pthread_t player_thread;
};

#endif /* __INTERFACE_H__ */
