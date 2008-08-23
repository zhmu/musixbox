#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "formBrowser.h"

using namespace std;

static char upbutton[8]    = { 0x08, 0x04, 0x02, 0x7f, 0x7f, 0x02, 0x04, 0x08 };
static char downbutton[8]  = { 0x08, 0x10, 0x20, 0x7f, 0x7f, 0x20, 0x10, 0x08 };
static char crossbutton[8] = { 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81 };

formBrowser::formBrowser(Interaction* in, std::string path)
	: Form(in)
{
	for (unsigned int i = 0; i < interaction->getHeight() / interaction->getTextHeight(); i++) {
		Label* l = new Label(0, i * interaction->getTextHeight(),
		                     interaction->getWidth() - 8, interaction->getTextHeight());
		l->setData(new int(i));
		dirlabel.push_back(l);
		add(l);
	}

	bLeave = new Image(interaction->getWidth() - 8, 0, 8, 8, crossbutton);
	bUp = new Image(interaction->getWidth() - 8, 8, 8, 8, upbutton);
	bDown = new Image(interaction->getWidth() - 8, interaction->getHeight() - 8, 8, 8, downbutton);
	add(bLeave); add(bUp); add(bDown);

	rootPath = path; selectedFile = "";
	currentPath = string(rootPath);

	rehash = true;
}

void
formBrowser::update()
{
	DIR* dir;
	struct dirent* dent;

	if (rehash) {
		dir = opendir(currentPath.c_str());
		if (dir == NULL) {
			/*
			 * Can't open folder - report this nicely.
			 */	
			dirlabel[0]->setText("Unable to open folder");
			dirlabel[1]->setText(currentPath);
			dirlabel[2]->setText("<interact to continue>");
			return;
		}

		/*
		 * Read all directory items and place them in a vector, which
		 * we will sort later.
		 */
		direntries.clear(); direntry_index = 0;
		while((dent = readdir(dir)) != NULL) {
			// Never show the current directory '.'
			if (!strcmp(dent->d_name, "."))
				continue;
			// Don't allow travelling below the root path
			if (!strcmp(dent->d_name, "..") && currentPath == rootPath)
				continue;
			direntries.push_back(dent->d_name);
		}
		closedir(dir);
		sort(direntries.begin(), direntries.end());
		rehash = false;
		first_index = 0;
	}

	/*
	 * Fill the screen until there is no more space.
	 */
	unsigned int last_index = first_index;
	unsigned int index = 0;
	while (last_index < direntries.size()) {
		if (index * interaction->getTextHeight() >= interaction->getHeight())
			break;

		dirlabel[index++]->setText(direntries[last_index++]);
	}

	/*
	 * Ensure unused items are properly nullified.
	 */
	while (index < interaction->getHeight() / interaction->getTextHeight()) {
		dirlabel[index++]->setText("");
	}
}


void
formBrowser::interact(Control* control)
{
	if (control == bLeave) {
		/* Stop button - return to main screen */
		selectedFile = "";
		close();
		return;
	}

	if (control == bUp || control == bDown) {
		unsigned int items_per_page = interaction->getHeight() / interaction->getTextHeight();
		if (control == bDown) {
			if (first_index + items_per_page <= direntries.size()) {
				first_index = (first_index + items_per_page) % direntries.size();
			} else {
				first_index = 0;
			}
		} else {
			if (first_index >= items_per_page) {
				first_index = first_index - items_per_page;
			} else {
				first_index = direntries.size() - items_per_page;
			}
		}
		return;
	}

	/* If we got here, it must have been an item */
	Label* l = reinterpret_cast<Label*>(control);
	if (l->getText() == "..") {
		/* Need to go one level lower, so strip
		 * off the last /path item */
		currentPath = string(currentPath.begin(), currentPath.begin() + currentPath.find_last_of("/"));
		rehash = true;
		return;
	}

	/*
	 * We have an item - construct full path to see
	 * if it's a file or not
	 */
	string path = currentPath + string("/") + l->getText();
	struct stat fs;
	if (stat(path.c_str(), &fs) < 0)
		return;
	if (S_ISDIR(fs.st_mode)) {
		/* It's a path, so enter it */
		currentPath = path;
		rehash = true;
		return;
	}

	/* We got a file! */
	direntry_index = *(reinterpret_cast<int*> (l->getData()));
	selectedPath = currentPath;
	selectedFile = path;
	close();
}

bool
formBrowser::getNextFile(std::string& file)
{
        /*
	 * Try to ascend through the directory vector to the next file - if
	 * there are no more files, just give up.
         */
        if (++direntry_index >= direntries.size())
                return false;

	file = selectedPath + "/" + direntries[direntry_index];
	return true;
}

bool
formBrowser::getPreviousFile(std::string& file)
{
        if (direntry_index <= 0)
                return false;

	file = selectedPath + "/" + direntries[--direntry_index];
	return true;
}
