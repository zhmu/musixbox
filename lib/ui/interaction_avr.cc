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

void*
avrRecvThread(void* ptr)
{
	InteractionAVR* avr = (InteractionAVR*)ptr;
	fd_set fds;
	bool touched = false, ready = false;
	int x = -1, y = -1;
	int minX = 1000, minY = 1000, maxX = 0, maxY = 0;
	int oldX = -1, oldY = -1;
	int reading = 0; // What are we going to read?
	unsigned char cmd = '0'; 
	unsigned char data = '0';
	int dp = -1; /* Datapart we have to read next */

#if 0
	/* Rink: this does not belong here... need to store them or something */
	minX = 52; minY = 68;
	maxX = 207; maxY = 176;
#else
	/* Dwight: this does not belong here... need to store them or something */
	minX = 113; minY = 170;
	maxX = 930; maxY = 853;
#endif

	while (!avr->isTerminating()) {
		/*
		 * Use the same descriptor every loop invocation - this ensures
		 * we do not try to test for values like -1.
		 */
		int fd = avr->getFD();
		if (fd < 0) 
			break;

		/* wait until we get word the AVR gets data */
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		if (select(fd + 1, &fds, (fd_set*)NULL, (fd_set*)NULL, NULL) < 0) 
			break;
		
		if(!FD_ISSET(fd, &fds)) 
			/* It was not for us this means we got interrupted by
			 * something, so it's game over time */
			break;

		/* Read next command */
		if (cmd == '0') {
			if (!(read(avr->getFD(), &cmd, 1))) {
				break;
			}
			else {
				if (cmd == CMD_TOUCH_COORDS) {
					dp = 1; // start with x lowbyte
				}
				/* Wait for the next byte to be reported */
				continue;
			}
		}

		/* Read data, datapart selected on dp */
		if (!(read(avr->getFD(), &data, 1))) 
			break;
		
		/* Data read was succesfull, store data */
		switch (dp) {
			/* Touchscreen coordinate storage */
			case 1: /* We received the lowbyte of the X-coordinate */
				x = data; // x-lowbyte
				dp++; // goto x-highbyte
				break;
			case 2: /* We received the highbyte of the X-coordinate */
				x |= (data<<8); // x-highbyte
				dp++; // goto y-lowbyte
				break;
			case 3: /* We received the lowbyte of the Y-coordinate */
				y = data; //y-lowbyte
				dp++; // goto y-highbyte
				break;
			case 4: /* We received the highbyte of the Y-coordinate */
				y |= (data<<8); // y-highbyte
				dp = -1; cmd = '0'; // get next command
				touched = true;
				//fprintf(stderr, "avr's x,y: %i, %i\n", x,y);
				break;

			/* Unknown data part */
			default: fprintf(stderr, "Unknown data part\n");
		}

		/* Check validity */
		if ((x < 0) || (y < 0)) touched = false;
		
		/* Process coordinate set? */	
		if (touched) {
			touched = false;

			/* dynamically adjust scaling */
			if (x < minX) minX = x; if (x > maxX) maxX = x;
			if (y < minY) minY = y; if (y > maxY) maxY = y;
			x -= minX; y -= minY;
			x = (int)((float)x * ((float)avr->getWidth()) / (float)(maxX - minX));
			y = (int)((float)y * ((float)avr->getHeight()) / (float)(maxY - minY));
			/* we need to invert the Y */
			y = avr->getHeight() - y;

			/*
			 * Only report changed coordinates - otherwise, the LCD is way
			 * too sensitive...
			 */
			if (oldX != x || oldY != y) {
				avr->setCoordinates(x, y);
				oldX = x; oldY = y;
				fprintf(stderr, "display's x,y: %i, %i\n", x,y);
			}
			
		}
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
		throw InteractionException(std::string("Unable to open device ") + device);

	tcflush(fd, TCIOFLUSH);

	if (tcgetattr(fd, &opt) < 0)
		throw InteractionException(std::string("tcgetattr() failure for ") + device);

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
		throw InteractionException(std::string("tcsetattr() failure for ") + device);

	pthread_create(&recvThread, NULL, avrRecvThread, this);

	displaydata = (unsigned char*)malloc((getHeight() / 8) * getWidth());
	if (displaydata == NULL)
		throw InteractionException(std::string("Out of memory"));

	currentDisplayData = (unsigned char*)malloc((getHeight() / 8) * getWidth());
	if (currentDisplayData == NULL)
		throw InteractionException(std::string("Out of memory"));

	memset(displaydata, 0, (getHeight() / 8) * getWidth());
	memset(currentDisplayData, 0, (getHeight() / 8) * getWidth());

	/* We do not know what's on the LCD, so write whenever we can! */
	dirty = 1;
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

	dirty = 0;
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

	dirty = 1;
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
