#include <stdio.h>
#include <string>
#include "ui/form.h"
#include "ui/control.h"

using namespace std;

Control::Control(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	this->form = NULL; this->x = x; this->y = y;
	this->width = width; this->height = height;
	this->data = NULL;
}

void
Control::putpixel(unsigned int x, unsigned int y, unsigned int c)
{
	if (form == NULL)
		return;
	form->putpixel(x, y, c);
}

void 
Control::puttext(unsigned int x, unsigned int y, string s)
{
	if (form == NULL)
		return;
	form->puttext(x, y, s);
}

bool
Control::isTouched(unsigned int cx, unsigned int cy)
{
	return (cx >= x && cx <= x + width &&
	        cy >= y && cy <= y + height);
}
