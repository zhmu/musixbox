#include "ui/label.h"
#include "ui/font.h"

void
Label::draw()
{
	puttext(x, y, text);
}
