#include <pthread.h>
#include <string>
#include "input.h"
#include "info.h"
#include "output.h"
#include "visualize.h"

#ifndef __PLAYER_H__
#define __PLAYER_H__

//! \brief Class 
class Player {
friend	void* player_wrapper(void* data);
public:
	/*! \brief Construct a player object
	 *  \param o Output object to use
	 *  \param v Visualizer object to use
	 *  \param it Input object to use
	 *  \param d Decoder object to use
	 *  \param in Information object to use
	 */
	inline Player(Output* o, Visualizer* v, Input* it, Decoder* d, Info* in) {
		output = o; visualizer = v; input = it; decoder = d; info = in;
		playerPaused = false; havePlayerThread = false;
	}

	/*! \brief Construct a player object
	 *  \param resource Resource to use
	 *  \param it Input object to use
	 *  \param d Decoder object to use
	 *  \param in Information object to use
	 *
	 *  The appropriate input, decoder and information objects are created
	 *  using DecoderFactory.
	 */
	Player(std::string resource, Output* o, Visualizer* v);

	/*! \brief Destructs the player object
	 *
	 *  The input, decoder and information objects will automatically be
	 *  deleted.
	 */
	~Player();

	//! \brief Start playing
	void play();

	/*! \brief Stop playing
	 *
	 *  Subsequent calling of start() will result in playing from the start.
	 */
	void stop();

	//! \brief Pause playing
	void pause();

	//! \brief Continue playing
	void cont();

	//! \brief Is the player paused?
	bool isPaused() { return playerPaused; }

	//! \brief Retrieve the associated information object
	Info* getInfo() { return info; }

	//! \brief Retrieve how long we have been playing
	unsigned int getPlayingTime();

	/*! \brief Retrieve the total time the track lasts
	 *
	 *  Zero is returned if the total time is not known.
	 */
	unsigned int getTotalTime();

	//! \brief Called if the player successfully terminated
	virtual void terminated() { }

protected:
	//! \brief Output object used
	Output* output;

	//! \brief Visualizer object used
	Visualizer* visualizer;

	//! \brief Input object used
	Input* input;

	//! \brief Decoder object used
	Decoder* decoder;

	//! \brief Information object used
	Info* info;

	//! \brief Is the player paused?
	bool playerPaused;

	//! \brief Do we have a player thread?
	bool havePlayerThread;

	//! \brief Player thread
	pthread_t playerThread;
};

#endif /* __PLAYER_H__ */
