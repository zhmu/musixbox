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
		pthread_mutex_init(&mtx_data, NULL);
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
	virtual ~Player();

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
	bool isPaused();

	//! \brief Retrieve the associated information object
	Info* getInfo();

	//! \brief Retrieve how long we have been playing
	unsigned int getPlayingTime();

	/*! \brief Retrieve the total time the track lasts
	 *
	 *  Zero is returned if the total time is not known.
	 */
	unsigned int getTotalTime();

	//! \brief Called if the player successfully terminated
	virtual void terminated() { }

	/*! \brief Called to handle pausing/unpausing
	 *
	 *  This is designed to be called from the playing thread context! It
	 *  only exists here since the calling thread needs the same
	 *  synchronization primitives which really shouldn't be public.
	 */
	void handleUnpause();

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

	//! \brief Mutex used to protect data fields
	pthread_mutex_t mtx_data;

	//! \brief Condition variable used to suspend the player
	pthread_cond_t cv_suspend;

private:
	//! \brief Implementation of play() where the mutex is already held
	void play_locked();

	//! \brief Implementation of stop() where the mutex is already held
	void stop_locked();

	//! \brief Implementation of cont() where the mutex is already held
	void cont_locked();

	//! \brief Implementation of pause() where the mutex is already held
	void pause_locked();
};

#endif /* __PLAYER_H__ */
