#include <map>
#include <string>
#include "folder.h"
#include "ui/form.h"
#include "ui/image.h"
#include "ui/label.h"

#ifndef __FORMBROWSER_H__
#define __FORMBROWSER_H__

class formBrowser : public Form {
public:
	formBrowser(Interaction* in, Folder* f);

	std::string getSelectedFile() { return selectedFile; }
	bool getNextFile(std::string& file);
	bool getPreviousFile(std::string& file);

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
	unsigned int first_index;

	bool dirty, rehash;
	std::string selectedFile;
	std::string selectedPath;

	//! \brief Map used to store per folder which page we were displaying
	std::map<std::string, unsigned int> cachedIndexMap;
};

#endif /* __FORMBROWSER_H__ */
