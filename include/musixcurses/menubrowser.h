#include <curses.h>
#include <map>
#include <string>
#include "core/folder.h"
#include "menu.h"

#ifndef __MENU_BROWSER_H__
#define __MENU_BROWSER_H__

//! \brief Implements a browsable directory menu
class MenuBrowser : public Menu {
public:
	inline MenuBrowser(WINDOW* w, Folder* f) : Menu(w) {
		folder = f;
	}

	//! \brief Restore position to where it was when exiting the folder
	void attemptReturnPosition();

	//! \brief Store the position of the current folder
	void storePosition();

protected:
	/*! \brief Retrieve textual representation of an item
	 *  \param num The item to retrieve
	 */
	virtual std::string getItem(unsigned int num);

	//! \brief Retrieve the number of items
	virtual unsigned int getNumItems();

private:
	//! \brief The folder we are browsing
	Folder* folder;

	//! \brief Map used to store per folder which page we were displaying
	std::map<std::string, unsigned int> cachedPositionMap;
};

#endif /* __MENU_BROWSER_H__ */
