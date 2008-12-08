#include <curses.h>
#include <string>
#include <vector>
#include "core/folder.h"
#include "core/info.h"
#include "core/output.h"
#include "core/decode.h"
#include "core/player.h"
#include "core/mixer.h"
#include "misc/playlist.h"
#include "lyrics.h"
#include "menubrowser.h"
#include "menuplaylist.h"

#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#define PAIR_STATUS		1
#define PAIR_BROWSER		2
#define PAIR_INFO		3
#define PAIR_DIALOG		4

#define MODE_BROWSER		0
#define MODE_PLAYLIST		1
#define MODE_LYRICS		2

//! \brief Provides user interaction
class Interface {
public:
	/*! \brief Constructs a new interface object
	 *  \param o Output object to use
	 *  \param m Mixer object to use
	 *  \param f Folder containing media to browse
	 *  \param l Lyrics object to use
	 *  \param resource Resource to play, or NULL
	 */
	Interface(Output* o, Mixer* m, Folder* f, Lyrics* l, const char* resource);

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
	/*! \brief Plays a resource
	 *  \param res Resource to play
	 */
	void playResource(std::string resource);

	//! \brief Retrieve mixer object
	inline Mixer* getMixer() { return mixer; }

	//! \brief Tretrieve player object
	Player* getPlayer() { return player; }

	//! \brief Play next track
	void next();

	//! \brief Play previus track
	void prev();

	/*! \brief Adds the given file or folder to the playlist
	 *  \param resource Resource to add
	 */
	void addToPlaylist(std::string resource);

	/*! \brief Removes the given file or folder from the playlist
	 *  \param resource Resource to remove
	 */
	void removeFromPlaylist(std::string resource);

	//! \brief Reposition all windows
	void reposition();

	/*! \brief Draw a message dialog
	 *  \param s Text to show
	 *  \param keymsg Show 'press any key to continue' message
	 */
	void drawDialog(std::string s, bool keymsg = false);

	//! \brief Remove the dialog
	void clearDialog();

	/*! \brief Displays a modal dialog
	 *  \param s Text to show
	 */
	void dialog(std::string s);

	/*! \brief Called if lyrics need fetching */
	void fetchLyrics();

	/*! \brief Redraw lyrics display */
	void drawLyrics();

private:
	//! \brief Fill status window
	void fillStatus();

	//! \brief Fill info window
	void fillInfo();

	/*! \brief Handle a common keystroke
	 *  \param c Keystroke to handle
	 */
	void handleCommonInput(int c);

	/*! \brief Handle a keystroke in the browsr
	 *  \param c Keystroke to handle
	 */
	void handleBrowserInput(int c);

	/*! \brief Handle a keystroke in the playlist
	 *  \param c Keystroke to handle
	 */
	void handlePlaylistInput(int c);

	/*! \brief Handle a keystroke in the lyrics browser
	 *  \param c Keystroke to handle
	 */
	void handleLyricsInput(int c);

	//! \brief Forcefully update all content
	void redraw();

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
	WINDOW* winMsg;

	//! \brief Playlist
	Playlist playlist;
	
	//! \brief Current display mode
	int mode;

	//! \brief Browser menu
	MenuBrowser* menuBrowser;

	//! \brief Playlist menu
	MenuPlaylist* menuPlaylist;

	//! \brief Lyrics object
	Lyrics* lyrics;

	//! \brief Are we playing from the playlist?
	bool playingFromList;

	//! \brief Do we show the show the help?
	bool showHelp;

	//! \brief First lyrics line to draw
	unsigned int first_lyrics_line;

	//! \brief If set, lyrics are dirty and need to be refetched
	bool dirtyLyrics;
};

#endif /* __INTERFACE_H__ */
