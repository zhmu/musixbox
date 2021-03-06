#include <vector>
#include "misc/playlistitem.h"

#ifndef __PLAYLIST_H__
#define __PLAYLIST_H__

//! \brief Maintains a playlist
class Playlist {
public:
	//! \brief Constructs a new playlist
	Playlist();

	//! \brief Destructs the playlist
	virtual ~Playlist();

	//! \brief Retrieve the item list
	inline std::vector<PlaylistItem*> getItems() { return items; }

	/*! \brief Add an item to the playlist
	 *  \parm it Item to be added
	 */
	void addItem(PlaylistItem* it);

	/*! \brief Remove an item from the playlist by number */
	void removeItem(unsigned int num);

	/*! \brief Remove an item from the playlist by resource	 */
	void removeItem(std::string resource);

	//! \brief Clears the playlist
	void clear();

	/*! \brief Get the number of the current play item */
	unsigned int getCurrentPlayItem() { return currentPlayItem; }

	/*! \brief Set the current play item */
	void setCurrentPlayItem(unsigned int n) { currentPlayItem = n; }

	//! \brief Get the number of the next item to play
	unsigned int getNextPlayItem() { return nextPlayItem; }

	//! \brief Set the next item to play */
	void setNextPlayItem(unsigned int n) { nextPlayItem = n; }

	/*! \brief Retrieves the next resources to play and advances pointer
	 * 
	 *  Returns an empty string if the playlist has finished.
	 */
	std::string getNextResource();

	/*! \brief Retrieves the current resource to play
	 *
	 *  Returns an empty string if the playlist has finished.
	 */
	std::string getCurrentResource();

protected:
	//! \brief List of the items
	std::vector<PlaylistItem*> items;

	//! \brief Item we are currently playing
	unsigned int currentPlayItem;

	//! \brief Item we will play next
	unsigned int nextPlayItem;
};

#endif /* __PLAYLIST_H__ */
