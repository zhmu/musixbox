#include "output.h"
#include "mixer.h"

#ifndef __OUTPUT_SWMIX_H__
#define __OUTPUT_SWMIX_H__

//! \brief Output plugin that uses a software mixer and passes it to another output plugin
class OutputSWMixer : public Output {
friend class MixerSW;
public:
	/*! \brief Construct software mixer provider
	 *  \param o True output provider to use
	 *
	 *  The output plugin is automatically destroyed when this object is destroyed.
	 */
	OutputSWMixer(Output* o);

	//! \brief Deinitializes the libao library
	~OutputSWMixer();

	/*! \brief Plays the assorted buffer
	 *  \param buf Buffer to play
	 *  \param len Length of the buffer
	 */
	void play(char* buf, size_t len);

protected:
	//! \brief Retrieve current volume setting
	inline unsigned int getVolume() { return volume; }

	/*! \brief Set current volume
	 *  \param vol New volume value
	 */
	void setVolume(unsigned int vol);

private:
	//! \brief The true output plugin to use
	Output* output;

	//! \brief Current volume
	unsigned int volume;
};

#endif /* __OUTPUT_SWMIX_H__ */
