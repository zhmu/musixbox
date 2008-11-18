#include <alsa/asoundlib.h>
#include "output.h"

#ifndef __OUTPUT_ALSA_H__
#define __OUTPUT_ALSA_H__

//! \brief Output using ALSA
class OutputALSA : public Output {
public:
	/*! \brief Construct the ALSA library output provider
	 *
	 *  The first output device is used.
	 */
	OutputALSA();

	//! \brief Deinitializes the ALSA library
	~OutputALSA();

	/*! \brief Plays the assorted buffer
	 *  \param buf Buffer to play
	 *  \param len Length of the buffer
	 */
	void play(char* buf, size_t len);

private:
	//! \brief ALSA device handle
	snd_pcm_t* sound_device;
};

#endif /* __OUTPUT_ALSA_H__ */
