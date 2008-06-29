#include <ao/ao.h>
#include "output.h"

#ifndef __OUTPUT_AO_H__
#define __OUTPUT_AO_H__

//! \brief Output using libao
class OutputAO : public Output {
public:
	OutputAO() : Output() {
		output_dev = NULL;
	}

	/*! \brief Initializes the libao library
	 *
	 *  The first output device is used.
	 */
	int init();

	//! \brief Deinitializes the libao library
	void done();

	/*! \brief Plays the assorted buffer
	 *  \param buf Buffer to play
	 *  \param len Length of the buffer
	 */
	void play(char* buf, size_t len);

private:
	//! \brief libao's output device 
	ao_device* output_dev;
};

#endif /* __OUTPUT_AO_H__ */
