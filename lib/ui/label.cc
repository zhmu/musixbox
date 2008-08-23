#include "ui/label.h"
#include "font.h"

void
Label::draw()
{
	puttext(x, y, text);
}
