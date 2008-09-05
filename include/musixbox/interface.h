#include <string>
#include <vector>
#include "core/folder.h"
#include "core/info.h"
#include "core/output.h"
#include "core/visualize.h"
#include "core/decode.h"
#include "core/mixer.h"
#include "ui/interaction.h"
#include "boxplayer.h"

#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#define INTERFACE_BROWSER_BAR_SIZE	10

class formBrowser;

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

	//! \brief Relinquish control to the operating system
	void relinquish();

private:
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
	formBrowser* browser;
};

#endif /* __INTERFACE_H__ */
