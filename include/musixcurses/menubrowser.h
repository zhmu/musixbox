#include <curses.h>
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
};

#endif /* __MENU_BROWSER_H__ */
