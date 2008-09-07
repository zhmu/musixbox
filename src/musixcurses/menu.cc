#include "menu.h"

using namespace std;

void
Menu::draw()
{
	int x;
	unsigned int line = 0;

	getmaxyx(window, num_lines, x);

	/* Ensure items that do not exist cannot be selected */
	if (sel_item >= getNumItems() && getNumItems() > 0)
		sel_item = getNumItems() - 1;

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

	/*
	 * If the user touched A-Z, attempt a lookup
	 */
	if (isalpha(c)) {
		c = tolower(c);
		lookup += c;
		return tryLookup();
		
	}

	/* We are no longer looking stuff up */
	lookup = "";

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

bool
Menu::tryLookup()
{
	size_t lookup_len = lookup.length();

	/*
	 * First of all, attempt to do a search from our current position; this
	 * is usually what the user is looking for
	 */
	for (unsigned int i = sel_item + 1; i < getNumItems(); i++) {
		if (!strncasecmp(getCompareItem(i).c_str(), lookup.c_str(), lookup_len)) {
			sel_item = i;
			return true;
		}
	}



	for (unsigned int i = 0; i < getNumItems(); i++) {
		if (!strncasecmp(getCompareItem(i).c_str(), lookup.c_str(), lookup_len)) {
			sel_item = i;
			return true;
		}
	}

	return false;
}
