#include "interaction.h"
#include "font.h"

void
Interaction::puttext(int x, int y, const char* s)
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
Interaction::gettextsize(const char* s, int* h, int* w)
{
}

void
Interaction::clear(int x, int y, int h, int w)
{
	for (int j = 0; j < h; j++)
		for (int i = 0; i < w; i++)
			putpixel(i + x, j + y, 0);
}

int
Interaction::getCoordinates(int* x, int* y)
{
	if (coordX == -1 && coordY == -1)
		return 0;
	*x = coordX; *y = coordY;
	 coordX = -1; coordY = -1;
	return 1;
}

