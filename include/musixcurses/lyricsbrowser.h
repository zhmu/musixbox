#include <curses.h>
#include <string>
#include "lyrics.h"

#ifndef __LYRICSBROWSER_H__
#define __LYRICSBROWSER_H__

//! \brief Implements the lyrics browser
class LyricsBrowser {
public:
	/*! \brief Constructs a lyrics browser
	 *  \param w Window to draw on
	 *  \param l Lyrics object to use
	 */
	inline LyricsBrowser(WINDOW* w, Lyrics* l) {
		window = w; lyrics = l; searching = false;
	}

	//! \brief Draw the lyrics
	void draw();

	/*! \brief Reset the browser status
	 *
	 *  This must be called if the lyrics object is updated.
	 */
	void reset();

	/*! \brief Handle input keystroke
	 *  \return true if handeled
	 *
	 *  If the key was handeled, draw must be called.
	 */
	bool handleInput(int c);

protected:
	/*! \brief Called if an attempt to look up a string is to be made */
	void tryLookup();

private:
	//! \brief Window used to draw
	WINDOW* window;

	//! \brief Lyrics object displayed
	Lyrics* lyrics;

	//! \brief First line displayed in the lyrics window
	unsigned int first_line;

	//! \brief Current lookup string
	std::string lookup;

	//! \brief Are we in search mode?
	bool searching;
};

#endif /* __LYRICSBROWSER_H__ */
