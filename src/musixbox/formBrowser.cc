#include "ui/form.h"
#include "ui/interaction.h"
#include "formBrowser.h"

using namespace std;

static char upbutton[8]    = { 0x08, 0x04, 0x02, 0x7f, 0x7f, 0x02, 0x04, 0x08 };
static char downbutton[8]  = { 0x08, 0x10, 0x20, 0x7f, 0x7f, 0x20, 0x10, 0x08 };
static char crossbutton[8] = { 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81 };

formBrowser::formBrowser(Interaction* in, Folder* f)
	: Form(in)
{
	for (unsigned int i = 0; i < (interaction->getHeight() - interaction->getTextHeight())  / interaction->getTextHeight(); i++) {
		Label* l = new Label(0, i * interaction->getTextHeight(),
		                     interaction->getWidth() - 8, interaction->getTextHeight());
		l->setData(new int(i));
		dirlabel.push_back(l);
		add(l);
	}

	unsigned int sz = interaction->getWidth() / 3;
	bUp =    new Image( 0, interaction->getHeight() - 8, sz, 8, upbutton);
	bDown =  new Image(sz, interaction->getHeight() - 8, sz, 8, downbutton);
	bLeave = new Image( 2 * sz, interaction->getHeight() - 8, sz, 8, crossbutton);
	add(bLeave); add(bUp); add(bDown);

	folder = f; rehash = true;
	selectedPath = folder->getPath();
}

void
formBrowser::update()
{
	if (rehash) {
		/*
		 * Attempt to look up the visiting page number - if it's not there, just
		 * default to the top.
		 */
		map<string, unsigned int>::iterator it = cachedIndexMap.find(folder->getPath());

		if(it != cachedIndexMap.end()) {
			first_index = it->second;
		} else {
			first_index = 0;
		}

		rehash = false;
	}

	/*
	 * Fill the screen until there is no more space.
	 */
	unsigned int last_index = first_index;
	unsigned int index = 0;
	while (last_index < folder->getEntries().size()) {
		if (index * interaction->getTextHeight() >= interaction->getHeight() - interaction->getTextHeight())
			break;

		dirlabel[index++]->setText(folder->getEntries()[last_index++]);
	}

	/*
	 * Ensure unused items are properly nullified.
	 */
	while (index < (interaction->getHeight()  - interaction->getTextHeight()) / interaction->getTextHeight()) {
		dirlabel[index++]->setText("");
	}
}

void
formBrowser::interact(Control* control)
{
	/*
	 * If we are rehashing, do not accept any interaction - this prevents
	 * the browser form attemping to navigate two subfolders because it
	 * got two interactions shortly after eachother while the screen was not
	 * yet updated.
	 */
	if (rehash)
		return;

	if (control == bLeave) {
		/* Stop button - return to main screen */
		cachedIndexMap[folder->getPath()] = first_index;
		selectedFile = "";
		close();
		return;
	}

	if (control == bUp || control == bDown) {
		unsigned int items_per_page = interaction->getHeight() / interaction->getTextHeight();
		if (control == bDown) {
			if (first_index + items_per_page <= folder->getEntries().size()) {
				first_index = (first_index + items_per_page) % folder->getEntries().size();
			} else {
				first_index = 0;
			}
		} else {
			if (first_index >= items_per_page) {
				first_index = first_index - items_per_page;
			} else {
				first_index = folder->getEntries().size() - items_per_page;
			}
		}
		return;
	}

	/* If we got here, it must have been an item */
	Label* l = reinterpret_cast<Label*>(control);
	if (l->getText() == "..") {
		/* Need to go one level lower */
		folder->goUp();
		rehash = true;
		return;
	}

	/* Rememember on which page we were in the current path */
	cachedIndexMap[folder->getPath()] = first_index;
	if (folder->isFolder(l->getText())) {
		/* It's a path, so enter it */
		folder->select(l->getText());
		rehash = true;
		return;
	}

	/* We got a file! */
	direntry_index = *(reinterpret_cast<int*> (l->getData()));
	selectedPath = folder->getPath();
	selectedFile = folder->getFullPath(l->getText());
	close();
}

bool
formBrowser::getNextFile(std::string& file)
{
        /*
	 * Try to ascend through the directory vector to the next file - if
	 * there are no more files, just give up.
         */
        if (++direntry_index >= folder->getEntries().size())
                return false;

	file = selectedPath + "/" + folder->getEntries()[direntry_index];
	return true;
}

bool
formBrowser::getPreviousFile(std::string& file)
{
        if (direntry_index <= 0)
                return false;

	file = selectedPath + "/" + folder->getEntries()[--direntry_index];
	return true;
}
