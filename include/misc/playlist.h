#include <vector>
#include "misc/playlistitem.h"

#ifndef __PLAYLIST_H__
#define __PLAYLIST_H__

//! \brief Maintains a playlist
class Playlist {
public:
	//! \brief Constructs a new playlist
	inline Playlist() {
		currentPlayItem = 0;
	}

	//! \brief Destructs the playlist
	virtual ~Playlist();

	//! \brief Retrieve the item list
	inline std::vector<PlaylistItem*> getItems() { return items; }

	/*! \brief Add an item to the playlist
	 *  \parm it Item to be added
	 */
	void addItem(PlaylistItem* it);

	/*! \brief Remove an item from the playlist */
	void removeItem(PlaylistItem* it);

	/*! \brief Get the number of the current play item */
	int getCurrentPlayItem();

protected:
	//! \brief List of the items
	std::vector<PlaylistItem*> items;

	//! \brief Item we are currently playing
	int currentPlayItem;
};

#endif /* __PLAYLIST_H__ */
