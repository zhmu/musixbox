#include <map>
#include <string>
#include "core/folder.h"
#include "ui/form.h"
#include "ui/image.h"
#include "ui/label.h"

#ifndef __FORMALPHABROWSER_H__
#define __FORMALPHABROWSER_H__

class formAlphaBrowser : public Form {
public:
	formAlphaBrowser(Interaction* in, Folder* f);

	inline unsigned char getSelectedChar() { return sel_ch; }

protected:
	void update();
	void interact(Control* control);

private:
	std::vector<Label*> label;
	Image* bLeave;

	unsigned char sel_ch;
};

#endif /* __FORMALPHABROWSER_H__ */
