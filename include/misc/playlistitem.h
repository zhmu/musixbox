#include <string>

#ifndef __PLAYLISTITEM_H__
#define __PLAYLISTITEM_H__

class PlaylistItem {
public:
	/*! \brief Constructs a playlist item
	 *  \param res Resource referring to the item
	 */
	inline PlaylistItem(std::string res) { resource = res; }

	//! \brief Retrieve the resource to play
	inline std::string getResource() { return resource; }

protected:
	//! \brief The item
	std::string resource;
};

#endif /* __PLAYLISTITEM_H__ */
