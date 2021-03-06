#include <string.h>
#include "menu.h"

using namespace std;

void
Menu::draw()
{
	unsigned int num_cols;
	unsigned int line = 0, search_line = 0 /* quench incorrect warning */;

	getmaxyx(window, num_lines, num_cols);

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

	/* If we are searching, claim the lowest line for ourselves */
	if (searching) {
		search_line = --num_lines;
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
		string s = getItem(first_item + line);
		if (s.length() + 2 < num_cols || num_cols < 7) {
			/*
		 	 * The string either fits on the screen, or we have
		 	 * no space to propery trim it - write it as-is.
		 	 */
			mvwprintw(window, line, 1, " %s ", s.c_str());
		} else {
			/*
			 * The string doesn't fit on the screen - chop off
			 * a part to make it fit.
			 */
			s = s.substr(0, num_cols - 7) + "...";
			mvwprintw(window, line, 1, " %s ", s.c_str());
		}
		if (first_item + line == sel_item)
			wattroff(window, A_REVERSE);
		line++;
	}
	if (searching)
		mvwprintw(window, search_line, 1, "search> %s ", lookup.c_str());
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
	 * If we are searching, handle adding/removing chars from the
	 * lookup buffer.
	 */
	if (searching) {
		if (isalnum(c) || isblank(c)) {
			c = tolower(c);
			lookup += c;
			tryLookup();
			return true;
		}
		if (c == KEY_BACKSPACE) {
			if (lookup.size() > 0) {
				lookup.erase(lookup.size() - 1);
			}
			return true;
		}
	}

	/* We are no longer looking stuff up */
	bool forceRedraw = searching;
	lookup = ""; searching = false;

	switch(c) {
		case 'k':
		case KEY_UP:
		case 'j':
		case KEY_DOWN:
			/* advance menu position */
			if (c == KEY_UP || c == 'k')
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
		case '/':
			searching = true;
			break;
		default:
			return forceRedraw;
	}
	return true;
}

void
Menu::tryLookup()
{
	size_t lookup_len = lookup.length();

	/*
	 * First of all, attempt to do a search from our current position; this
	 * is usually what the user is looking for.
	 */
	for (unsigned int i = sel_item + 1; i < getNumItems(); i++) {
		if (!strncasecmp(getCompareItem(i).c_str(), lookup.c_str(), lookup_len)) {
			sel_item = i;
			return;
		}
	}

	/*
	 * If this failed, wade through the entire list.
	 */
	for (unsigned int i = 0; i < getNumItems(); i++) {
		if (!strncasecmp(getCompareItem(i).c_str(), lookup.c_str(), lookup_len)) {
			sel_item = i;
			return;
		}
	}
}

void
Menu::setSelectedItem(unsigned int num)
{
	if (num >= getNumItems())
		return;
	sel_item = num;
}

void
Menu::reset()
{
	first_item = 0; sel_item = 0;
	lookup = ""; searching = false;
}
