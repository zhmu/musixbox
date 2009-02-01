#include <map>
#include <string>
#include "core/folder.h"
#include "ui/form.h"
#include "ui/image.h"
#include "ui/label.h"

#ifndef __FORMBROWSER_H__
#define __FORMBROWSER_H__

#define FORMBROWSER_CODE_CANCELED 0
#define FORMBROWSER_CODE_SELECTED 1
#define FORMBROWSER_CODE_GOUP     2

class formBrowser : public Form {
public:
	formBrowser(Interaction* in, Folder* f);

	std::string getSelectedFile() { return selectedFile; }
	bool getNextFile(std::string& file);
	bool getPreviousFile(std::string& file);

	inline void setFilterChar(unsigned char ch) { filterChar = ch; }

	int run();

protected:
	void update();
	void interact(Control* control);

private:
	std::vector<Label*> dirlabel;
	Image* bDown;
	Image* bUp;
	Image* bLeave;

	Folder* folder;

	unsigned int direntry_index;
	unsigned int current_page;

	unsigned char filterChar;

	bool dirty, rehash, nextpage, new_visit;
	std::string selectedFile;
	std::string selectedPath;

	//! \brief Map used to store per folder which page we were displaying
	std::map<std::string, unsigned int> cachedIndexMap;

	std::string getIndexKey();
};

#endif /* __FORMBROWSER_H__ */
