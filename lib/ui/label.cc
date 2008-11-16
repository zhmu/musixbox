#include "ui/label.h"
#include "ui/font.h"

using namespace std;

void
Label::draw()
{
	puttext(x, y, text);
}
