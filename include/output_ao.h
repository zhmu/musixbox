#include <ao/ao.h>
#include "output.h"

#ifndef __OUTPUT_AO_H__
#define __OUTPUT_AO_H__

//! \brief Output using libao
class OutputAO : public Output {
public:
	/*! \brief Construct the libao library output provider
	 *
	 *  The first output device is used.
	 */
	OutputAO();

	//! \brief Deinitializes the libao library
	~OutputAO();

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
