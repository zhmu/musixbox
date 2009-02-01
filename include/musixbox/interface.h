#include <string>
#include <vector>
#include "core/folder.h"
#include "core/info.h"
#include "core/output.h"
#include "core/visualize.h"
#include "core/decode.h"
#include "core/mixer.h"
#include "misc/playlist.h"
#include "ui/interaction.h"
#include "boxplayer.h"

#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#define INTERFACE_BROWSER_BAR_SIZE	10

class formAlphaBrowser;
class formBrowser;
class formPlayer;
class formPlaylist;

//! \brief Provides user interaction
class Interface {
friend  class formPlayer;

public:
	/*! \brief Constructs a new interface object
	 *  \param i Interaction object to use
	 *  \param o Output object to use
	 *  \param m Mixer object to use
	 *  \param f Folder containing media to browse
	 *  \param resource Resource to play, or NULL
	 */
	Interface(Interaction* i, Output* o, Mixer* m, Folder* f, const char* resource);

	//! \brief Destructs the interface object
	~Interface();

	//! \brief Run the interface 
	void run();

	//! \brief Called if the current track finished playing
	void trackDone();

	//! \brief Retrieve the playlist
	Playlist* getPlaylist() { return &playlist; }

	//! \brief Current file being played
	std::string getCurrentFile() { return currentFile; }


	/*! \brief Add a resource in the current folder to the playlist
	 *  \param resource Resource to add
	 *
	 *  Items in the subfolder are added as well.
	 */
	void addToPlaylist(std::string resource);

	/*! \brief Start playing form the playlist
	 *  \param num Entry to play
	 *
	 *  After [num] is done, [num+1] will be played, etc.
	 */
	void startPlaylist(int num);

	/*! \brief Do we have a current plaything?
	 *
	 *  If this returns true, it means we are playing or paused on a
	 *  resource.
	 */
	bool havePlayer() { return player != NULL; }

protected:
	/*! \brief Plays a resource
	 *  \param resource Resource to play
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

private:
	//! \brief Current folder charachter
	unsigned char currentFolderChar;

	//! \brief Interaction object we are using
	Interaction* interaction;

	//! \brief Folder where all media resides
	Folder* folder;

	//! \brief Output object
	Output* output;

	//! \brief Visualization object
	Visualizer* visualizer;

	//! \brief Mixer object
	Mixer* mixer;

	//! \brief Player object
	Player* player;

	//! \brief Current file playing
	std::string currentFile;

	//! \brief Browser form
	formBrowser* fBrowser;

	//! \brief Player form
	formPlayer* fPlayer;

	//! \brief Alpha browser form
	formAlphaBrowser* fAlphaBrowser;

	//! \brief Playlist form
	formPlaylist* fPlaylist;

	//! \brief Playlist in use
	Playlist playlist;

	//! \brief Are we playing the playlist?
	bool playingFromPlaylist;
};

#endif /* __INTERFACE_H__ */
