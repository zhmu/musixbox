#include <sys/select.h>
#include <err.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "core/exceptions.h"
#include "ui/interaction_avr.h"

#undef DEBUG

using namespace std;

/*
 * struct COMMANDMAP is used to look up the corresponding aergument length
 * per command.
 */
struct COMMANDMAP {
	uint8_t	cmd;
	uint8_t len;
} commandmap[] = {
	{ CMD_NONE,   0 },
	{ CMD_COORDS, 4 },
};

void*
avrRecvThread(void* ptr)
{
	InteractionAVR* avr = (InteractionAVR*)ptr;
	fd_set fds;
	uint8_t curCmd = CMD_NONE;
	uint8_t curByte, numBytes;
	uint8_t argBuf[CMD_MAX_DATA_LENGTH];

	while (!avr->isTerminating()) {
		/*
		 * Use the same descriptor every loop invocation - this ensures
		 * we do not try to test for values like -1.
		 */
		int fd = avr->getFD();
		if (fd < 0) 
			break;

		/* Wait until there is something to report */
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		if (select(fd + 1, &fds, (fd_set*)NULL, (fd_set*)NULL, NULL) < 0) 
			break;
		
		if(!FD_ISSET(fd, &fds)) 
			/*
			 * We got interrupted, but no data is available. This
			 * generally means the select() got interrupted by a
			 * signal - so just attempt again (the isTerminating()
		 	 * condition gets us out of the loop if needed)
			 */
			break;

		if (curCmd == CMD_NONE) {
			/*
			 * We do not have a command, so the first byte we read is the
			 * command
			 */
			read(avr->getFD(), &curCmd, 1);

			/* Look up the command length */
			unsigned int idx;
			for (idx = 0; idx < sizeof(commandmap) / sizeof(struct COMMANDMAP); idx++)
				if (commandmap[idx].cmd == curCmd)
					break;
			if (idx == sizeof(commandmap) / sizeof(struct COMMANDMAP)) {
				/* No such command */
#ifdef DEBUG
				fprintf(stderr, "InteractionAVR: received unknown command 0x%02x, ignored\n", curCmd);
#endif
				curCmd = CMD_NONE;
				break;
			}
			curByte = 0; numBytes = commandmap[idx].len;
			if (numBytes >= CMD_MAX_DATA_LENGTH) {
#ifdef DEBUG
				fprintf(stderr, "InteractionAVR: command 0x%02x has argument length %u > maximum %u, adjusting!\n", curCmd, numBytes, CMD_MAX_DATA_LEN);
#endif
				numBytes = CMD_MAX_DATA_LENGTH;
			}
			/* If we need to read data, do it */
			if (numBytes > 0)
				continue;
		}

		/* Read extra data if needed */
		if (curByte < numBytes) {
			if (!(read(avr->getFD(), &argBuf[curByte++], 1))) 
				break;
			if (curByte < numBytes)
				continue;
		}

		/* We have a complete data packet - process it */
		switch (curCmd) {
			/* Touchscreen coordinate storage */
			case CMD_COORDS:
				avr->handleTouch(argBuf);
				break;
			default:
				/*
				 * Length is know, but no action. Ignore the
				 * to be on the safe side
				 */
				break;
		}
		curCmd = CMD_NONE;
	}

	return NULL;
}

InteractionAVR::InteractionAVR(const char* device)
{
	struct termios opt;

	haveReceivingThread = false;
	terminating = false;

	fd = open(device, O_RDWR | O_NOCTTY| O_NDELAY);
	if (fd < 0)
		throw InteractionException(string("Unable to open device ") + device);

	tcflush(fd, TCIOFLUSH);

	if (tcgetattr(fd, &opt) < 0)
		throw InteractionException(string("tcgetattr() failure for ") + device);

	/* 57600 baud */
	cfsetispeed(&opt, B57600);
	cfsetospeed(&opt, B57600);

	opt.c_cflag |= CLOCAL | CREAD;

	/* 8 bits, no parity, 1 stop bit */
	opt.c_cflag &= ~(PARENB | PARODD | CSTOPB | CSIZE);
	opt.c_cflag |= CS8;

	/* no hardware/software flow control */
	opt.c_cflag &= ~CRTSCTS;
	opt.c_iflag &= ~(IXON | IXOFF | IXANY);
	opt.c_iflag |= IGNBRK;

	/* raw input */
	opt.c_lflag = 0;
	opt.c_oflag = 0;

	opt.c_cc[VTIME] = 1;
	opt.c_cc[VMIN] = 60;

	if (tcsetattr(fd, TCSANOW, &opt) < 0)
		throw InteractionException(string("tcsetattr() failure for ") + device);

	displaydata = (unsigned char*)malloc((getHeight() / 8) * getWidth());
	if (displaydata == NULL)
		throw InteractionException(string("Out of memory"));

	currentDisplayData = (unsigned char*)malloc((getHeight() / 8) * getWidth());
	if (currentDisplayData == NULL)
		throw InteractionException(string("Out of memory"));

	/* We want a clear display, so ensure we overwrite every little byte on there */
	memset(displaydata, 0, (getHeight() / 8) * getWidth());
	memset(currentDisplayData, 0xFF, (getHeight() / 8) * getWidth());

	/* XXX: store me in config file */
	minX = 91; maxX = 1846; minY = 223; maxY = 1748;

	/* Finally, launch the thread to cope with incoming data */
	pthread_create(&recvThread, NULL, avrRecvThread, this);

	/* We do not know what's on the LCD, so write whenever we can! */
	dirty = true;
}

InteractionAVR::~InteractionAVR()
{
	if (fd >= 0) {
		close(fd);
		fd = -1;
	}

	if (haveReceivingThread) {
		terminating = true;
		pthread_join(recvThread, NULL);
	}

	if (currentDisplayData == NULL)
		free(currentDisplayData);

	if (displaydata == NULL)
		free(displaydata);
}


void
InteractionAVR::yield()
{
	if (!dirty)
		return;

	/*
	 * If we have outstanding updates, write them page by page, controller
	 * by controller. This is actually so much faster than sending updates
	 * byte-by-byte, it's almost scary :-)
	 */
	unsigned char* srcptr = displaydata;
	unsigned char* dstptr = currentDisplayData;
	unsigned char* src;
	int isDirty;
	for (unsigned int page = 0; page < getHeight() / 8; page++) {
		for (unsigned char ic = 0; ic < 2; ic++) {
			isDirty = 0; src = srcptr;
			for (unsigned int i = 0; i < getWidth() / 2; i++) {
				if (*srcptr != *dstptr)
					isDirty = 1;
				*dstptr++ = *srcptr++;
			}
			if (isDirty) {
				/* need to copy this entire page over */
				writeAVRPage(ic, page, src);
			}
		}
	}

	dirty = false;
}

void
InteractionAVR::putpixel(unsigned int x, unsigned int y, unsigned int c)
{
	if (x >= getWidth() || y >= getHeight())
		return;

	if (c)
		displaydata[x + (getWidth() * (y / 8))] |= 1 << (y % 8);
	else
		displaydata[x + (getWidth() * (y / 8))] &= ~(1 << (y % 8));

	dirty = true;
}

void
InteractionAVR::writeAVRPage(unsigned char ic, unsigned char page, unsigned char* data)
{
	unsigned char a;

	a = 0x80 | (ic ? 0x40 : 0) | page;
	if (!write(fd, &a, 1))
		return;
	if (!write(fd, data, 64))
		return;
}

void
InteractionAVR::handleTouch(uint8_t* buf)
{
	uint16_t x, y;

	x = buf[0] | (buf[1] << 8);
	y = buf[2] | (buf[3] << 8);

#if 0
	/* dynamically adjust scaling */
	if (x < minX) minX = x; if (x > maxX) maxX = x;
	if (y < minY) minY = y; if (y > maxY) maxY = y;
#endif

#ifdef DEBUG
	fprintf(stderr, "touch: got bytes x=%u,y=%u (minX=%u,maxX=%u,minY=%u,maxY=%u)=>", x, y, minX, maxX, minY, maxY);
#endif

	/* Check validity */
	if (x < minX || y < minY || x > maxX || y > maxY) {
#ifdef DEBUG
		fprintf(stderr, " ignored\n");
#endif
		return;
	}
		
	/* Process coordinate set? */	
	x -= minX; y -= minY;
	x = (int)((float)x * ((float)getWidth()) / (float)(maxX - minX));
	y = (int)((float)y * ((float)getHeight()) / (float)(maxY - minY));
	/* We need to invert the X */
	x = getWidth() - x;
#ifdef DEBUG
	fprintf(stderr, " display's x,y: %i, %i\n", x,y);
#endif

	/* All is well => report the coordinate */
	setInteraction(x, y, INTERACTION_TYPE_NORMAL);
}
