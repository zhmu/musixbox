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
	Decoder();
	~Decoder();

	//! \brief Decode the input stream until the end
	virtual int run() = 0;

	//! \brief Attach an Input object with the decoder
	inline void setInput(Input* i) { input = i; }
	//! \brief Attach an Output object with the decoder
	inline void setOutput(Output* o) { output = o; }
	//! \brief Attach a Visualizer object with the decoder
	inline void setVisualizer(Visualizer* v) { visualizer = v; }

	//! \brief Get rid of the decoder
	inline void terminate() { terminating = true; }

protected:
	Input* input;
	Output* output;
	Visualizer* visualizer;

	//! \brief Buffer available to the object, 
	char* out_buffer;

	//! \brief Are we currently terminating?
	bool terminating;
};

#endif /* __DECODE_H__ */
