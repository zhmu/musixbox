#include <sys/types.h>

#ifndef __OUTPUT_H__
#define __OUTPUT_H__

//! \brief Abstract class to provide audio output
class Output {
public:
	//! \brief Destructs the output plugin
	inline virtual ~Output() { };

	/*! \brief Plays a buffer
	 *  \param buf Buffer to play
	 *  \param len Number of bytes to play
	 *
	 *  The buffer is expected to contain signed 16 bit PCM stereo data,
	 *  which may be manipulated by the output plugin as it deems
	 *  necessary.
	 */
	virtual void play(char* buf, size_t len) = 0;
};

#endif /* __OUTPUT_H__ */
