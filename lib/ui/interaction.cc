#include <stdlib.h>
#include "ui/font.h"
#include "ui/interaction.h"

using namespace std;

void
Interaction::puttext(unsigned int x, unsigned int y, const char* s)
{
	for (; *s; s++) {
		struct CHARACTER* c = &theFont[(unsigned char)*s];
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
Interaction::gettextsize(const char* s, unsigned int* h, unsigned int* w)
{
	unsigned int calch = 8 /* XXX */, calcw = 0;

	for (; *s; s++) {
		struct CHARACTER* c = &theFont[(unsigned char)*s];
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
	intX = x; intY = y; intType = type;
}

void 
Interaction::flushInteraction()
{
	intType = INTERACTION_TYPE_NONE;
}
