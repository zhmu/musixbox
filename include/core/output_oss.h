#include "output.h"

#ifndef __OUTPUT_OSS_H__
#define __OUTPUT_OSS_H__

//! \brief Output using libao
class OutputOSS : public Output {
public:
	/*! \brief Construct the libao library output provider
	 *
	 *  The first output device is used.
	 */
	OutputOSS();

	//! \brief Deinitializes the libao library
	~OutputOSS();

	/*! \brief Plays the assorted buffer
	 *  \param buf Buffer to play
	 *  \param len Length of the buffer
	 */
	void play(char* buf, size_t len);

private:
	//! \brief File descriptor used for playback
	int fd;
};

#endif /* __OUTPUT_OSS_H__ */
