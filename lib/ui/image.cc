#include "ui/image.h"

void
Image::draw()
{
	if (image == NULL)
		return;

	for (unsigned int i = 0; i < 8; i++) {
		for (unsigned int j = 0; j < 8; j++) {
			putpixel(i + x, j + y, (image[i] & (1 << j)) ? 1 : 0);
		}
	}
}
