#include <string.h>
#include <stdio.h>
#include "ui/form.h"
#include "ui/interaction.h"
#include "formAlphaBrowser.h"
#include "images.h"

using namespace std;

formAlphaBrowser::formAlphaBrowser(Interaction* in, Folder* folder)
	: Form(in)
{
	char msg[512];
	char chars[26];

	/* Make a list of all chars we have */
	memset(chars, 0, sizeof(chars));
	for (unsigned int i = 0; i < folder->getEntries().size(); i++) {
		const char* entry = folder->getEntries()[i].c_str();
		unsigned char ch = toupper(entry[0]);
		chars[ch - 'A'] = (ch >= 'A' && ch <= 'Z');
	}

	/* Place the chars we need on the screen */
	for (int j = 0; j < 4; j++)
		for (int i = 0; i < 7; i++) {
			int ch = j * 7 + i;
			int x = i * (interaction->getWidth() / 7);
			x += (interaction->getWidth() - (6 * (interaction->getWidth() / 7))) / 4;
			int y = j * (interaction->getTextHeight() + interaction->getTextHeight() / 4);
			if (ch < 26) {
				/* If we have no folders with this char, skip it */
				if (!chars[ch])
					continue;
				Label* l = new Label(x, y, 16, 16);
				snprintf(msg, sizeof(msg), "%c", 'A' + ch);
				l->setText(msg);
				l->setData(new int(ch));
				label.push_back(l);
				add(l);
			}
			if (ch ==  27) {
				bLeave = new Image(x, y, 16, 16, Images::leave());
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
	Label* l = static_cast<Label*>(control);
	sel_ch = 'A' + *(reinterpret_cast<int*> (l->getData()));
	close();
}
