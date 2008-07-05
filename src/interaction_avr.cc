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

	/* 4800 baud */
	cfsetispeed(&opt, B4800);
	cfsetospeed(&opt, B4800);

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

	displaydata = (char*)malloc((getHeight() / 8) * getWidth());
	if (displaydata == NULL)
		return 0;

	currentDisplayData = (char*)malloc((getHeight() / 8) * getWidth());
	if (currentDisplayData == NULL)
		return 0;

	memset(displaydata, 0, (getHeight() / 8) * getWidth());
	memset(currentDisplayData, 0, (getHeight() / 8) * getWidth());

	/* Force a clear screen */
	writeAVR(0xff, 0xff, 0xff);
	dirty = 0;
	return 1;
}

void
InteractionAVR::yield()
{
	if (!dirty)
		return;

	/*
	 * If we have outstanding updates, check whatever we believe is
	 * on the AVR and send it updates - this eliminates the need to
	 * let the AVR read the LCD memory, which is slow...
	 */
	for (int i = 0; i < (getHeight() / 8) * getWidth(); i++) {
		if (currentDisplayData[i] != displaydata[i]) {
			currentDisplayData[i] = displaydata[i];
			uint8_t ic = (i % getWidth()) > 63 ? 0x40 : 0;
			uint8_t address = i % 64;
			uint8_t page = i / getWidth();
			writeAVR(0x80 | ic | page,	/* ic/page */
				 address,		/* address*/
				 displaydata[i]);	/* data */
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
InteractionAVR::putpixel(int x, int y, int c)
{
	if (x < 0 || y < 0 || x >= getWidth() || y >= getHeight())
		return;

	if (c)
		displaydata[x + (getWidth() * (y / 8))] |= 1 << (y % 8);
	else
		displaydata[x + (getWidth() * (y / 8))] &= ~(1 << (y % 8));

	dirty = 1;
}

void
InteractionAVR::writeAVR(unsigned char a, unsigned char b, unsigned char c)
{
	fd_set fds;
	char ch;
	char buf[3] = { a, b, c };

	int len = write(fd, buf, 3);
	if (!len)
		return;

	/* Wait for the acknowledgement charachter */
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	if (select(fd + 1, &fds, (fd_set*)NULL, (fd_set*)NULL, NULL) < 0)
		return;
	if(!FD_ISSET(fd, &fds))
		return;
	read(fd, &ch, 1);
}
