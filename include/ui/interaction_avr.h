#include "interaction.h"

#ifndef __INTERACTION_AVR_H__
#define __INTERACTION_AVR_H__

#define CMD_TOUCH_COORDS	0x01
#define CMD_TOUCH_COORD_X	0x04
#define CMD_TOUCH_COORD_Y	0x05

//! \brief Provides interaction using AVR, serially connected
class InteractionAVR : public Interaction {
friend 	void* avrRecvThread(void*);
public:
	/*! \brief Constructs a new interaction provider
	 *  \param device Serial device name
	 */
	InteractionAVR(const char* device);

	//! \brief Destructs the AVR interaction provider
	~InteractionAVR();

	//! \brief Returns the height of the AVR window
	inline unsigned int getHeight() { return 64; }

	//! \brief Returns the width of the AVR window
	inline unsigned int getWidth() { return 128; }

	//! \brief Used to handle interactions
	void yield();

	void putpixel(unsigned int x, unsigned int y, unsigned int c);
	bool isScrollingOK() { return false; }

protected:
	void writeAVRPage(unsigned char ic, unsigned char page, unsigned char* data);

	//! \brief Do we have to terminate?
	bool isTerminating() { return terminating; }

	//! \brief Retrieve the file descriptor used for serial access
	int getFD() { return fd; };

private:
	int fd;

	int dirty;

	//! \brief Working contents of display data
	unsigned char* displaydata;

	//! \brief Current contents of display data
	unsigned char* currentDisplayData;

	//! \brief Have we launched the receiver thread
	bool haveReceivingThread;

	//! \brief Is the receiving thread terminating
	bool terminating;

	//! \brief Receiving thread
	pthread_t recvThread;
};

#endif /* __INTERACTION_AVR_H__ */
