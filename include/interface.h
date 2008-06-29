#include <pthread.h>
#include <string>
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
		output = NULL; input = NULL; decoder = NULL; visualization = NULL;
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

	//! \brief Launch the player
	void launchPlayer();

	//! \brief Stops playing
	void stop();

	/*! \brief Plays a file
	 *  \param fname File to play
	 */
	void play(std::string fname);
	

private:
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
	Visualizer* visualization;

	//! \brief Do we have a playing thread?
	bool hasPlayerThread;

	//! \brief Playing thread
	pthread_t player_thread;
};

#endif /* __INTERFACE_H__ */
