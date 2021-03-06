#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "ui/font.h"
#include "ui/interaction.h"

using namespace std;

void
Interaction::puttext(unsigned int x, unsigned int y, const char* s, FONT* font)
{
	y += font->height;
	for (; *s; s++) {
		struct CHARACTER* c = &font->chars[(unsigned char)*s];
		for (int j = 0; j < c->height; j++) {
			for (int i = 0; i <= c->width; i++) {
				uint8_t d = c->data[j];
				if (d & (1 << i))
					putpixel(x + i, y + j - c->yshift, 1);
			}
		}
		x += c->advance_x;
	}
}

void
Interaction::gettextsize(const char* s, unsigned int* h, unsigned int* w, FONT* font)
{
	unsigned int calch = font->height, calcw = 0;

	for (; *s; s++) {
		struct CHARACTER* c = &font->chars[(unsigned char)*s];
		calcw += c->advance_x;
	}

	if (h != NULL)
		*h = calch;
	if (w != NULL)
		*w = calcw;
}

void
Interaction::clear(unsigned int x, unsigned int y, unsigned int h, unsigned int w)
{
	for (unsigned int j = 0; j < h; j++)
		for (unsigned int i = 0; i < w; i++)
			putpixel(i + x, j + y, 0);
}

bool
Interaction::getInteraction(unsigned int* x, unsigned int* y, unsigned int* type)
{
	if (intType == INTERACTION_TYPE_NONE)
		return false;
	*x = intX; *y = intY; *type = intType;
	intType = INTERACTION_TYPE_NONE;
	return true;
}

void
Interaction::setInteraction(unsigned int x, unsigned int y, unsigned int type)
{
	unsigned long delta = calculateInteractionDelta();
	gettimeofday(&intTimestamp, NULL);

	if (delta < 50000UL)
		return;

	intX = x; intY = y; intType = type;
}

void 
Interaction::flushInteraction()
{
	intType = INTERACTION_TYPE_NONE;
}

unsigned long
Interaction::calculateInteractionDelta()
{
	struct timeval now;
	gettimeofday(&now, NULL);

	bool P1 = intTimestamp.tv_sec  <  now.tv_sec;
	bool P2 = intTimestamp.tv_usec < now.tv_usec;

	if (P1 && P2) {
		return ((now.tv_sec - intTimestamp.tv_sec) * 1000000UL) +
		        now.tv_usec - intTimestamp.tv_usec;
	}

	if (P1 && !P2) {
		return ((now.tv_sec - intTimestamp.tv_sec) * 1000000UL) -
		        intTimestamp.tv_usec + now.tv_usec;
	}

	if (!P1 && P2) {
		return (now.tv_usec - intTimestamp.tv_usec);
	}

	/*
	 * If we reach this, time has gone backwards, since !P1 && !P2 holds.
	 */
	return 0;
}

unsigned int
Interaction::getTextHeight()
{
	return DEFAULT_FONT.height;
}
