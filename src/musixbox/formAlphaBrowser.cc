#include "ui/form.h"
#include "ui/interaction.h"
#include "formAlphaBrowser.h"

using namespace std;

#if 0
static char upbutton[8]    = { 0x08, 0x04, 0x02, 0x7f, 0x7f, 0x02, 0x04, 0x08 };
static char downbutton[8]  = { 0x08, 0x10, 0x20, 0x7f, 0x7f, 0x20, 0x10, 0x08 };
static char crossbutton[8] = { 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81 };
#endif
static char imgLeave[8] = { 0x20, 0x72, 0xaa, 0x22, 0x22, 0x22, 0x3e, 0x00 };

formAlphaBrowser::formAlphaBrowser(Interaction* in)
	: Form(in)
{
	char msg[512];

	for (int j = 0; j < 4; j++)
		for (int i = 0; i < 7; i++) {
			int ch = j * 7 + i;
			int x = i * (interaction->getWidth() / 7) + 4 /* fontwidth/2 */;
			int y = j * (interaction->getTextHeight() * 2) +
			            (interaction->getTextHeight() / 2);
			if (ch < 26) {
				Label* l = new Label(x, y, 16, 16);
				snprintf(msg, sizeof(msg), "%c", 'A' + ch);
				l->setText(msg);
				l->setData(new int(ch));
				label.push_back(l);
				add(l);
			}
			if (ch ==  27) {
				bLeave = new Image(x, y, 8, 8, imgLeave);
				add(bLeave);
			}
	}

	sel_ch = 0;
}

void
formAlphaBrowser::update()
{
}

void
formAlphaBrowser::interact(Control* control)
{
	if (control == bLeave) {
		sel_ch = 0;
		close();
		return;
	}

	/* If we got here, it must be one of the A...Z labels */
	Label* l = reinterpret_cast<Label*>(control);
	sel_ch = 'A' + *(reinterpret_cast<int*> (l->getData()));
	close();
}
