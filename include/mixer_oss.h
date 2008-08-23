#include <string>
#include "mixer.h"

#ifndef __MIXER_OSS_H__
#define __MIXER_OSS_H__

//! \brief Handles volume setting for OpenSound System mixer devices
class MixerOSS : public Mixer {
public:
	/*! \brief Initialize the OSS mixer
	 *  \param dev Device to use
	 */
	MixerOSS(std::string device);

	//! \brief Cleans up the mixer device
	~MixerOSS();

	//! \brief Retrieve volume value
	unsigned int getVolume();

	/*! \brief Set volume
	 *  \param val Value to set the volume to
	 */
	void setVolume(unsigned int val);

private:
	//! \brief File descriptor used for the mixer
	int fd;
};

#endif /* __MIXER_OSS_H__ */
