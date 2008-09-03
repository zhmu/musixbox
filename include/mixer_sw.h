#include <string>
#include "mixer.h"
#include "output_swmixer.h"

#ifndef __MIXER_SW_H__
#define __MIXER_SW_H__

/*! \brief Handles volume setting for software mixing
 *
 *   This needs to be used in conjuction with OutputSWMixer
 */;
class MixerSW : public Mixer {
public:
	/*! \brief Initialize the software mixer
	 *  \param o Software mixing output to use
	 */
	inline MixerSW(OutputSWMixer* o) { output = o; }

	//! \brief Cleans up the mixer device
	inline ~MixerSW() { };

	//! \brief Retrieve volume value
	unsigned int getVolume() { return output->getVolume(); }

	/*! \brief Set volume
	 *  \param val Value to set the volume to
	 */
	void setVolume(unsigned int val) { output->setVolume(val); }

private:
	//! \brief Software mixing output provider
	OutputSWMixer* output;
};

#endif /* __MIXER_SW_H__ */
