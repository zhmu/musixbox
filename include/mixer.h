#include <string>

#ifndef __MIXER_H__
#define __MIXER_H__

//! \brief Abstract class to provide a volume interface
class Mixer {
public:
	/*! \brief Construct a new mixer
	 *  \param dev Device to use
	 */
	Mixer(std::string device) { }

	//! \brief Retrieve volume value
	virtual unsigned int getVolume() = 0;

	/*! \brief Set volume
	 *  \param val Value to set the volume to
	 */
	virtual void setVolume(unsigned int val) = 0;
};

#endif /* __MIXER_H__ */
