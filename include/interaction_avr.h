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
	inline int getHeight() { return 64; }

	//! \brief Returns the width of the AVR window
	inline int getWidth() { return 128; }

	//! \brief Used to handle interactions
	void yield();

	void putpixel(int x, int y, int c);

protected:
	void writeAVR(unsigned char a, unsigned char b, unsigned char c);

private:
	int fd;
	int dirty;

	//! \brief Working contents of display data
	char* displaydata;

	//! \brief Current contents of display data
	char* currentDisplayData;
};

#endif /* __INTERACTION_AVR_H__ */
