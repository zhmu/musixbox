#include <curses.h>
#include <string>
#include <vector>
#include "core/folder.h"
#include "core/info.h"
#include "core/output.h"
#include "core/decode.h"
#include "core/player.h"
#include "core/mixer.h"

#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#define PAIR_STATUS		1
#define PAIR_BROWSER		2
#define PAIR_INFO		3

//! \brief Provides user interaction
class Interface {
public:
	/*! \brief Constructs a new interface object
	 *  \param o Output object to use
	 *  \param m Mixer object to use
	 *  \param f Folder containing media to browse
	 *  \param resource Resource to play, or NULL
	 */
	Interface(Output* o, Mixer* m, Folder* f);

	//! \brief Destructs the interface object
	~Interface();

	//! \brief Run the interface 
	void run();

	//! \brief Called if the current track finished playing
	void trackDone();

	//! \brief Called if a status update is requested
	void requestUpdate();

	//! \brief Called if a resize has been detected
	void signalResize();

protected:
	//! \brief Plays the current file
	void playFile();

	//! \brief Retrieve mixer object
	inline Mixer* getMixer() { return mixer; }

	//! \brief Tretrieve player object
	Player* getPlayer() { return player; }

	//! \brief Play next track
	void next();

	//! \brief Play previus track
	void prev();

private:
	//! \brief Fill status window
	void fillStatus();

	//! \brief Fill info window
	void fillInfo();

	//! \brief Fill directory browser
	void fillBrowser();

	/*! \brief Handle a keystroke
	 *  \param c Keystroke to handle
	 */
	void handleInput(int c);

	//! \brief Folder where all media resides
	Folder* folder;

	//! \brief Output object
	Output* output;

	//! \brief Mixer object
	Mixer* mixer;

	//! \brief Player object
	Player* player;

	//! \brief Current file playing
	std::string currentFile;

	//! \brief ncurses window references
	WINDOW* winStatus;
	WINDOW* winBrowser;
	WINDOW* winInfo;

	//! \brief Top item shown in browser window
	unsigned int browser_first_item;

	//! \brief Currently selected item in browser window
	unsigned int browser_sel_item;

	//! \brief Number of lines in browser window
	unsigned int browser_lines;
};

#endif /* __INTERFACE_H__ */
