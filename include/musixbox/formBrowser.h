#include <map>
#include <string>
#include "core/folder.h"
#include "ui/form.h"
#include "ui/image.h"
#include "ui/label.h"
#include "interface.h"

#ifndef __FORMBROWSER_H__
#define __FORMBROWSER_H__

#define FORMBROWSER_CODE_CANCELED 0
#define FORMBROWSER_CODE_SELECTED 1
#define FORMBROWSER_CODE_GOUP     2
#define FORMBROWSER_CODE_QUEUED   3

#define CD_TYPE_LABEL 0
#define CD_TYPE_QUEUE 1

class formBrowserControlData {
public:
	formBrowserControlData(int t, int v) {
		type = t; value = v;
	};
	
	int getType() { return type; }
	int getValue() { return value; }

private:
	int type, value;
};

class formBrowser : public Form {
public:
	formBrowser(Interaction* in, Interface* iface, Folder* f);

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
	std::vector<Image*> queuebtn;
	Image* bDown;
	Image* bUp;
	Image* bLeave;

	Interface* interface;
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
