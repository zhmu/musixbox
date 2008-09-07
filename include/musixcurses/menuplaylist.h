#include "core/folder.h"
#include "misc/playlist.h"
#include "menu.h"

#ifndef __MENU_PLAYLIST_H__
#define __MENU_PLAYLIST_H__

//! \brief Implements a browsable playlist menu
class MenuPlaylist : public Menu {
public:
	inline MenuPlaylist(WINDOW* w, Playlist* p) : Menu(w) {
		playlist = p;
	}


protected:
	/*! \brief Retrieve textual representation of an item
	 *  \param num The item to retrieve
	 */
	virtual std::string getItem(unsigned int num);

	/*! \brief Retrieve textual representation of an item for comparison purposes
	 *  \param num The item to retrieve
	 */
	virtual std::string getCompareItem(unsigned int num);

	//! \brief Retrieve the number of items
	virtual unsigned int getNumItems();

private:
	//! \brief The playlist we are serving
	Playlist* playlist;
};

#endif /* __MENU_PLAYLIST_H__ */
