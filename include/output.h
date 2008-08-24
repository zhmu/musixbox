#ifndef __OUTPUT_H__
#define __OUTPUT_H__

//! \brief Abstract class to provide audio output
class Output {
public:
	/*! \brief Plays a buffer
	 *  \param buf Buffer to play
	 *  \param len Number of bytes to play
	 *
	 *  The buffer is expected to contain 16 bit PCM stereo data.
	 */
	virtual void play(char* buf, size_t len) = 0;
};

#endif /* __OUTPUT_H__ */
