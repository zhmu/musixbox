#include <err.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "font.h"
#include "interaction_avr.h"

void*
avrRecvThread(void* ptr)
{
	InteractionAVR* avr = (InteractionAVR*)ptr;
	fd_set fds;
	int coord = 0, x, y;
	int minX = 1000, minY = 1000, maxX = 0, maxY = 0;
	int oldX = -1, oldY = -1;

	/* Rink: this does not belong here... need to store them or something */
	minX = 52; minY = 68;
	maxX = 207; maxY = 176;

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
		unsigned char a;
		if (!read(avr->getFD(), &a, 1))
			break;
		if (coord > 0) {
			coord--;
			if (coord == 1) {
				/* got X */
				x = a;
			} else {
				/* got Y */
				y = a;

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
				}
			}
		} else if (a == 0x40)
			coord = 2;
	}
	return NULL;
}

InteractionAVR::InteractionAVR(const char* device)
{
	haveReceivingThread = false;
	terminating = false;

	fd = open(device, O_RDWR | O_NOCTTY| O_NDELAY);
	if (fd < 0)
		throw NULL;

	pthread_create(&recvThread, NULL, avrRecvThread, this);
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
}


int
InteractionAVR::init()
{
	struct termios opt;

	tcflush(fd, TCIOFLUSH);

	if (tcgetattr(fd, &opt) < 0)
		return 0;

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
		return 0;

	displaydata = (unsigned char*)malloc((getHeight() / 8) * getWidth());
	if (displaydata == NULL)
		return 0;

	currentDisplayData = (unsigned char*)malloc((getHeight() / 8) * getWidth());
	if (currentDisplayData == NULL)
		return 0;

	memset(displaydata, 0, (getHeight() / 8) * getWidth());
	memset(currentDisplayData, 0, (getHeight() / 8) * getWidth());

	/* We do not know what's on the LCD, so write whenever we can! */
	dirty = 1;
	return 1;
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
InteractionAVR::done()
{
	if (currentDisplayData == NULL)
		free(currentDisplayData);

	if (displaydata == NULL)
		free(displaydata);
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
