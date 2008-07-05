#include <err.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "font.h"
#include "interaction_avr.h"

#define DEVICE "/dev/cuaU0"

int
InteractionAVR::init()
{
	struct termios opt;

	fd = open(DEVICE, O_RDWR | O_NOCTTY| O_NDELAY);
	if (fd < 0)
		return 0;

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

	mouseX = -1; mouseY = -1; dirty = 0;
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
InteractionAVR::puttext(int x, int y, const char* s)
{
	for (; *s; s++) {
		struct CHARACTER* c = &theFont[*s];
		for (int i = 0; i < c->width; i++) {
			for (int j = 0; j < 8 /*c->height*/; j++) {
				unsigned char d = c->data[i * ((j / 8) + 1)];
				if (d & (1 << j))
					putpixel(x + i, y + j + (c->height - c->yshift), 1);
			}
		}
		x += c->advance_x;
	}
}

void
InteractionAVR::gettextsize(const char* s, int* h, int* w)
{
}

int
InteractionAVR::getCoordinates(int* x, int* y)
{
	if (mouseX == -1 && mouseY == -1)
		return 0;
	*x = mouseX; *y = mouseY;
	 mouseX = -1; mouseY = -1;
	return 1;
}

void
InteractionAVR::clear(int x, int y, int h, int w)
{
	for (int j = 0; j < h; j++)
		for (int i = 0; i < w; i++)
			putpixel(i + x, j + y, 0);
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

	//usleep(5500);
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	if (select(fd + 1, &fds, (fd_set*)NULL, (fd_set*)NULL, NULL) < 0)
		return;
	if(!FD_ISSET(fd, &fds))
		return;
	if (read(fd, &ch, 1) <= 0)
		return;
	return;

fail:
	printf("can't write to port\n");
}
