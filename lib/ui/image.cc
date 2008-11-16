#include "ui/image.h"

using namespace std;

void
Image::draw()
{
	if (image == NULL)
		return;

	unsigned int drawX = x + (width -  8) / 2;
	unsigned int drawY = y + (height - 8) / 2;

	for (unsigned int i = 0; i < 8; i++) {
		for (unsigned int j = 0; j < 8; j++) {
			putpixel(i + drawX, j + drawY, (image[i] & (1 << j)) ? 1 : 0);
		}
	}
}
