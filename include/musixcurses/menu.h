#include <curses.h>
#include <string>

#ifndef __MENU_H__
#define __MENU_H__

//! \brief Implements a friendly curses menu
class Menu {
public:
	/*! \brief Construct a new menu
	 *  \param w Window used for drawing
	 */
	inline Menu(WINDOW* w) {
		first_item = 0; sel_item = 0; num_lines = 0;
		window = w;
	}

	//! \brief Draw menu
	void draw();

	/*! \brief Handle input keystroke
	 *  \return true if handeled
	 *
	 *  If the key was handeled, draw must be called.
	 */
	bool handleInput(int c);

	//!  \brief Resets the menu to cope with new content
	void reset() {
		first_item = 0; sel_item = 0;
	}

	//! \brief Retrieve the currently selected item
	unsigned int getSelectedItem() { return sel_item; }

protected:
	/*! \brief Retrieve textual representation of an item
	 *  \param num The item to retrieve
	 */
	virtual std::string getItem(unsigned int num) = 0;

	//! \brief Retrieve the number of items
	virtual unsigned int getNumItems() = 0;

private:
	//! \brief Window used to draw
	WINDOW* window;

	//! \brief Top item shown in menu
	unsigned int first_item;

	//! \brief Currently selected item
	unsigned int sel_item;

	//! \brief Number of lines in window
	unsigned int num_lines;
};

#endif /* __MENU_H__ */
