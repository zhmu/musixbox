#include "input.h"
#include "output.h"
#include "visualize.h"

#ifndef __DECODE_H__
#define __DECODE_H__

//! \brief Size of a temporary buffer every decoder may use
#define DECODER_OUTBUF_SIZE	8192

/*! \brief Abstract decoder object
 *
 *  A decoder is reponsible for reading data from an Input object, decoding it
 *  to 16 bit PCM stereo data and feeding the raw PCM data to an Output object
 *  and Visualizer object.
 */
class Decoder {
public:
	Decoder(Input* i, Output* o, Visualizer* v);
	virtual ~Decoder();

	//! \brief Decode the input stream until the end
	virtual int run() = 0;

	//! \brief Get rid of the decoder
	inline void terminate() { terminating = true; }

	//! \brief Is the decoder forcefully terminating?
	inline bool isTerminating() { return terminating; }

	//! \brief Retrieve number of seconds this file is playing
	inline int getPlayingTime() { return playingtime; }

	//! \brief Retrieve number of seconds this file lasts
	inline int getTotalTime() { return totaltime; }

protected:
	Input* input;
	Output* output;
	Visualizer* visualizer;

	//! \brief Buffer available to the object, 
	char* out_buffer;

	//! \brief Are we currently terminating?
	bool terminating;

	//! \brief Number of seconds we have been playing
	int playingtime;

	//! \brief Number of seconds this song lasts
	int totaltime;
};

#endif /* __DECODE_H__ */
