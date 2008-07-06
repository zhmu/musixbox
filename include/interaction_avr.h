#include "interaction.h"

#ifndef __INTERACTION_AVR_H__
#define __INTERACTION_AVR_H__

//! \brief Provides interaction using AVR, serially connected
class InteractionAVR : public Interaction {
public:
	/*! \brief Constructs a new interaction provider
	 *  \param device Serial device name
	 */
	InteractionAVR(const char* device);
	 
	//! \brief Initialize AVR interaction provider
	int init();

	//! \brief Deinitialize AVR interaction provider
	void done();

	//! \brief Returns the height of the AVR window
	inline unsigned int getHeight() { return 64; }

	//! \brief Returns the width of the AVR window
	inline unsigned int getWidth() { return 128; }

	//! \brief Used to handle interactions
	void yield();

	void putpixel(unsigned int x, unsigned int y, unsigned int c);

protected:
	void writeAVRPage(unsigned char ic, unsigned char page, unsigned char* data);

private:
	int fd;
	int dirty;

	//! \brief Working contents of display data
	unsigned char* displaydata;

	//! \brief Current contents of display data
	unsigned char* currentDisplayData;
};

#endif /* __INTERACTION_AVR_H__ */
