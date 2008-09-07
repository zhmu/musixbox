#include "core/player.h"
#include "misc/playlistitem.h"

PlaylistItem::PlaylistItem(std::string res)
{
	resource = res;
        artist = ""; album = ""; title = "";

	/*
	 * XXX This is kind of evil: we simply launch a player and retrieve
	 * it's information object, after which we throw it away...
	 */
	Player* player = new Player(resource, NULL, NULL);
	if (player->getInfo()) {
		if (player->getInfo()->getArtist() != NULL)
			artist = player->getInfo()->getArtist();
		if (player->getInfo()->getAlbum() != NULL)
			album = player->getInfo()->getAlbum();
		if (player->getInfo()->getTitle() != NULL)
			title = player->getInfo()->getTitle();
		
	}
	delete player;
}

std::string
PlaylistItem::getDisplayText()
{
	if (title == "")
		return resource;

	return artist + " / " + album + " - " + title;
}
