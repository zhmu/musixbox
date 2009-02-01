#include <string>
#include "core/info.h"

#ifndef __PLAYLISTITEM_H__
#define __PLAYLISTITEM_H__

class PlaylistItem {
public:
	/*! \brief Constructs a playlist item
	 *  \param res Resource referring to the item
	 */
	PlaylistItem(std::string res);

	//! \brief Retrieve the resource to play
	inline std::string getResource() { return resource; }

	//! \brief Retrieve item's artist
	inline std::string getArtist() { return artist; }

	//! \brief Retrieve item's album
	inline std::string getAlbum() { return album; }

	//! \brief Retrieve item's title
	inline std::string getTitle() { return title; }

	//! \brief Retrieve the text to display for this item
	std::string getDisplayText();

protected:
	//! \brief The item
	std::string resource;

        //! \brief Name of the artist
        std::string artist;

        //! \brief Name of the album
        std::string album;

        //! \brief Title of the song
        std::string title;
};

#endif /* __PLAYLISTITEM_H__ */
