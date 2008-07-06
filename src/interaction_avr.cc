#include <err.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "font.h"
#include "interaction_avr.h"

InteractionAVR::InteractionAVR(const char* device)
{
	fd = open(device, O_RDWR | O_NOCTTY| O_NDELAY);
	if (fd < 0)
		throw NULL;
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
	if (fd >= 0)
		close(fd);

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
	fd_set fds;
	unsigned char a;
	int len;

	a = 0x80 | (ic ? 0x40 : 0) | page;
	if (!write(fd, &a, 1))
		return;
	if (!write(fd, data, 64))
		return;

	/* Wait for the acknowledgement charachter */
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	if (select(fd + 1, &fds, (fd_set*)NULL, (fd_set*)NULL, NULL) < 0)
		return;
	if(!FD_ISSET(fd, &fds))
		return;
	read(fd, &a, 1);
}
