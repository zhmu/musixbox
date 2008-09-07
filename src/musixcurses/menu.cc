#include "menu.h"

using namespace std;

void
Menu::draw()
{
	int x;
	unsigned int line = 0;

	getmaxyx(window, num_lines, x);

	/*
	 * Ensure that whatever is selected fits on the screen by adjusting the
	 * scroll position as needed.
	 */
	if (sel_item < first_item ||
	   sel_item >= first_item + num_lines) {
		if (num_lines / 2 <= sel_item)
			first_item = sel_item - (num_lines / 2);
		else
			first_item = 0;
	}

	/*
	 * Keep placing folder entries on the screen one-by-one until we either
	 * run out of entries or run out of space.
	 */
	werase(window);
	while (first_item + line < getNumItems()) {
		if (line >= (unsigned int)num_lines)
			break;
		if (first_item + line == sel_item)
			wattron(window, A_REVERSE);
		mvwprintw(window, line, 1, " %s ",
			getItem(first_item + line).c_str());
		if (first_item + line == sel_item)
			wattroff(window, A_REVERSE);
		line++;
	}
	wrefresh(window);
}

bool
Menu::handleInput(int c)
{
	string item;

	/* If there are no items, we can't do anything */
	if (getNumItems() == 0)
		return false;

	switch(c) {
		case KEY_UP:
		case KEY_DOWN:
			/* advance menu position */
			if (c == KEY_UP)
				sel_item = (sel_item > 0) ? sel_item - 1 : getNumItems() - 1;
			else
				sel_item = (sel_item + 1) % getNumItems();
			break;
		case KEY_NPAGE:
			first_item += (num_lines - 1);
			sel_item += (num_lines - 1);
			if (sel_item >= getNumItems())
				sel_item = getNumItems() - 1;
			break;
		case KEY_PPAGE:
			if (sel_item < (num_lines - 1)) {
				sel_item = 0;
			} else {
				first_item -= (num_lines - 1);
				sel_item -= (num_lines - 1);
			}
			break;
		case KEY_HOME:
			sel_item = 0;
			break;
		case KEY_END:
			sel_item = getNumItems() - 1;
			break;
		default:
			return false;
	}
	return true;
}