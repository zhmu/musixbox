#include <stdint.h>
#include "interaction.h"

#ifndef __INTERACTION_AVR_H__
#define __INTERACTION_AVR_H__

/* Commands send by the AVR to us */
#define CMD_NONE		0x00
#define CMD_COORDS		0x01
#define CMD_SYNC		0xf0
#define CMD_SYNCED		0xf1
#define CMD_DRAWN		0xf2

/* Maximum argument length */
#define CMD_MAX_DATA_LENGTH	0x10

//! \brief Provides interaction using AVR, serially connected
class InteractionAVR : public Interaction {
friend 	void* avrRecvThread(void*);
public:
	/*! \brief Constructs a new interaction provider
	 *  \param device Serial device name
	 */
	InteractionAVR(const char* device);

	//! \brief Destructs the AVR interaction provider
	virtual ~InteractionAVR();

	//! \brief Returns the height of the AVR window
	inline unsigned int getHeight() { return 64; }

	//! \brief Returns the width of the AVR window
	inline unsigned int getWidth() { return 128; }

	//! \brief Used to handle interactions
	void yield();

	void putpixel(unsigned int x, unsigned int y, unsigned int c);

protected:
	void writeAVRPage(unsigned char ic, unsigned char page, unsigned char* data);

	//! \brief Do we have to terminate?
	bool isTerminating() { return terminating; }

	//! \brief Retrieve the file descriptor used for serial access
	int getFD() { return fd; };

	//! \brief Handle a touch coordinate request
	void handleTouch(uint8_t* buf);

	//! \brief Synchronized the AVR device with us
	void sync();

	//! \brief Called if we got a synced charachter
	void setSynced() { synced = true; }

	//! \brief Called if we got a drawn command
	void setDrawn() { drawn = true; }

private:
	//! \brief File descriptor used for serial access
	int fd;

	//! \brief Need to update the display?
	bool dirty;

	//! \brief Have we gotten a sync charachter?
	bool synced;

	//! \brief Have we drawn the data?
	bool drawn;

	//! \brief Working contents of display data
	unsigned char* displaydata;

	//! \brief Current contents of display data
	unsigned char* currentDisplayData;

	//! \brief Have we launched the receiver thread
	bool haveReceivingThread;

	//! \brief Is the receiving thread terminating
	bool terminating;

	//! \brief Minimum X coordinate on the touch
	unsigned int minX;

	//! \brief Maximum X coordinate on the touch
	unsigned int maxX;

	//! \brief Minimum Y coordinate on the touch
	unsigned int minY;

	//! \brief Maximum Y coordinate on the touch
	unsigned int maxY;

	//! \brief Receiving thread
	pthread_t recvThread;
};

#endif /* __INTERACTION_AVR_H__ */
