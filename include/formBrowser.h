#include "ui/form.h"
#include "ui/image.h"
#include "ui/label.h"

#ifndef __FORMBROWSER_H__
#define __FORMBROWSER_H__

class formBrowser : public Form {
public:
	formBrowser(Interaction* in, std::string path);

	std::string getSelectedFile() { return selectedFile; }
	bool getNextFile(std::string& file);
	bool getPreviousFile(std::string& file);

protected:
	void update();
	void interact(Control* control);

private:
	Image* bDown;
	Image* bUp;
	Image* bLeave;

	std::vector<Label*> dirlabel;
	std::vector<std::string> direntries;
	unsigned int direntry_index;
	unsigned int first_index;

	bool dirty, rehash;
	std::string currentPath;
	std::string rootPath;
	std::string selectedFile;
	std::string selectedPath;
};

#endif /* __FORMBROWSER_H__ */
