#include "interaction.h"
#include "font.h"

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
}

void
Interaction::clear(unsigned int x, unsigned int y, unsigned int h, unsigned int w)
{
	for (unsigned int j = 0; j < h; j++)
		for (unsigned int i = 0; i < w; i++)
			putpixel(i + x, j + y, 0);
}

int
Interaction::getCoordinates(unsigned int* x, unsigned int* y)
{
	if (coordX == -1 && coordY == -1)
		return 0;
	*x = (unsigned int)coordX; *y = (unsigned int)coordY;
	 coordX = -1; coordY = -1;
	return 1;
}

