#include <algorithm>
#include <string.h>
#include "lyricsbrowser.h"
#include "interface.h"

using namespace std;

void
LyricsBrowser::reset()
{
	first_line = 0;
}

void
LyricsBrowser::draw()
{
	unsigned int line = 0, y = 0;
	unsigned int lyrics_line = 0 /* quench incorrect warning */;
	unsigned int num_lines, num_cols;

	getmaxyx(window, num_lines, num_cols);

	/* If we are searching, claim the last line for our prompt */
	if (searching)
		lyrics_line = --num_lines;

	/*
	 * Sanitize the first line shown - this simplifies the search and
	 * browse logic significantly.
	 */
	if (lyrics->getNumLines() > num_lines) {
		if (first_line + num_lines > lyrics->getNumLines()) {
			first_line = lyrics->getNumLines() - num_lines;
		}
	} else {
		first_line = 0;
	}

        werase(window);

	string::size_type cur_index = 0;
	while (cur_index != string::npos && y < num_lines) {
		/* Isolate the next line */
		string s;
		string::size_type pos = lyrics->getLyrics().find_first_of("\n", cur_index);
		if (pos != string::npos) {
			s = lyrics->getLyrics().substr(cur_index, pos - cur_index);
			cur_index = pos + 1;
		} else {
			s = lyrics->getLyrics().substr(cur_index);
			cur_index = pos;
		}
		if (line++ < first_line)
			continue;

		if (lookup != "") {
			/*
			 * A lookup string is available - we need to wade through
			 * the lyrics string and highlight any matches.
			 *
			 * Note: since C++ doesn't seem to have a case insensitive
			 * string::find() function, we transform the string we
		 	 * use for searching to all-lowercase.
			 */
			string searchstring = s;
			transform(searchstring.begin(), searchstring.end(), searchstring.begin(), ::tolower);

			string::size_type index = 0;
			unsigned int x = 1;
			while (1) {
				string::size_type pos = searchstring.find(lookup, index);
				if (pos == string::npos)
					break;

				/* we have found a match! */
				x += drawLyric(x, y, s.substr(index, pos - index).c_str());
				wattrset(window, COLOR_PAIR(PAIR_HILIGHT));
				mvwprintw(window, y, x, "%s", s.substr(pos, lookup.size()).c_str());
				wattrset(window, COLOR_PAIR(PAIR_BROWSER));
				x += lookup.size();
				index = pos + lookup.size();
			}
			drawLyric(x, y, s.substr(index).c_str());
		} else {
			drawLyric(1, y, s.c_str());
		}
		y++;
	}
	if (searching)
		mvwprintw(window, lyrics_line, 1, " search> %s", lookup.c_str());
	wrefresh(window);
}

bool
LyricsBrowser::handleInput(int c)
{
	unsigned int num_lines, num_cols;

	/*
	 * If we are searching, handle adding/removing of charachters from
	 * the lookup buffer.
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

	getmaxyx(window, num_lines, num_cols);

	switch(c) {
		case 'j':
		case KEY_DOWN:
			first_line++;
			break;
		case 'k':
		case KEY_UP:
			if (first_line > 0) {
				first_line--;
			}
			break;
		case KEY_NPAGE:
			first_line += (num_lines - 1);
			break;
		case KEY_PPAGE:
			if (first_line < (num_lines - 1)) {
				first_line  = 0;
			} else {
				first_line -= (num_lines - 1);
			}
			break;
		case KEY_HOME:
			first_line = 0;
			break;
		case KEY_END:
			first_line = lyrics->getNumLines() - 1;
			break;
		case '/':
			searching = true;
			lookup = "";
			break;
		case 0x0a: /* enter */
			searching = false;
			break;
		default:
			return false;
	}
	return true;
}

void
LyricsBrowser::tryLookup()
{
	bool gotFirstMatch = false;
	unsigned int first_match = 0 /* silence unjust warning */;
	unsigned int y = 0;
	unsigned int num_lines, num_cols;

	/*
	 * As the logic may be a bit non-intuitive, this comment tries to
	 * explain the basic idea. The issue is that the lyrics we have
	 * are just a single string, and we have to isolate any lyrics lines
	 * manually out of it.
	 *
	 * However, if we are searching, there are two possible scenarios:
	 *
	 * 1) The lookup text is at our current screen or futher down
	 * 2) The lookup text is at the top of the lyrics and no longer shown
	 *
	 * Since we don't want to go through the trouble of iterating through
	 * the lyrics twice, we always store the first reference we have. If
	 * we subsequently find lyrics that are on our current screen or below,
	 * we jump to them. Otherwise, we use the first reference and display
	 * those.
	 *
	 * Finally, beware that this is not a speed optimalization, but rather
	 * an attempt to produce cleaner code (even though the latter may be
	 * in vain due to this block of comments needed to explain the code :-))
	 */
	getmaxyx(window, num_lines, num_cols);

	string::size_type cur_index = 0;
	while (cur_index != string::npos) {
		string s;

		/* Isolate the next line */
		string::size_type pos = lyrics->getLyrics().find_first_of("\n", cur_index);
		if (pos != string::npos) {
			s = lyrics->getLyrics().substr(cur_index, pos - cur_index);
			cur_index = pos + 1;
		} else {
			s = lyrics->getLyrics().substr(cur_index);
			cur_index = pos;
		}
		transform(s.begin(), s.end(), s.begin(), ::tolower);

		if (s.find(lookup) != string::npos) {
			/*
			 * This line matches! If we didn't have a first match,
			 * use this one.
			 */
			if (!gotFirstMatch) {
				first_match = y; gotFirstMatch = true;
			}

			/* If this line is visible or futher down, jump to it */
			if (y >= first_line) {
				first_line = y;
				return;
			}
		}
		y++;
	}

	/*
	 * OK, the lookup string was not found further down. Thus, if we have
	 * a first match, jump to it.
	 */
	if (gotFirstMatch) {
		first_line = first_match;
	}
}

unsigned int
LyricsBrowser::drawLyric(unsigned int x, unsigned int y, const char* s)
{
	unsigned int num = 0;
	while (*s != '\0') {
		if (*s == '<') {
			/*
			 * OK, this may be a opening '<'. Look for the
			 * terminating closing '>'.
			 */
			const char* ptr = strchr(s, '>');
			if (ptr == NULL)
				break;
			string str = string(s + 1, ptr - s - 1);
			/*
			 * 'str' contains the tag itself, i.e. 'b' or '/b'. We need
			 * to lowercase 'str' and take a corresponding course
			 * of action (emphasize / restore attribute.
			 */
			transform(str.begin(), str.end(), str.begin(), ::tolower);
			if (str == "i")
				wattrset(window, COLOR_PAIR(PAIR_BROWSER) | A_BOLD);
			if (str == "b")
				wattrset(window, COLOR_PAIR(PAIR_BROWSER) | A_STANDOUT);
			if (str == "/i" || str == "/b")
				wattrset(window, COLOR_PAIR(PAIR_BROWSER));
			s = (ptr + 1);
			continue;
		}

		mvwprintw(window, y, x, "%c", *s);
		num++; x++; s++;
	}

	if (*s != '\0') {
		/* We can draw whatever portion of text is there */
		mvwprintw(window, y, x, "%s", s);
		num += strlen(s);
	}
	return num;
}
