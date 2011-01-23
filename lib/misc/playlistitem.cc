#include "core/decoderfactory.h"
#include "misc/playlistitem.h"

using namespace std;

PlaylistItem::PlaylistItem(string res)
{
	Input* input;
	Decoder* decoder;
	Info* info;

	resource = res;
	artist = ""; album = ""; title = "";

	/*
	 * Construct a decoder and an information object. We simply pass NULL as the
	 * Player, Output and Visualizer objects as we aren't interested in playing
	 * the item, only the Info objection.
	 */
	DecoderFactory::construct(resource, NULL, NULL, NULL, &input, &decoder, &info);
	if (info != NULL) {
		if (info->getArtist() != NULL)
			artist = info->getArtist();
		if (info->getAlbum() != NULL)
			album = info->getAlbum();
		if (info->getTitle() != NULL)
			title = info->getTitle();
		delete info;
	}
	delete decoder;
	delete input;
	
}

string
PlaylistItem::getDisplayText()
{
	if (title == "")
		return resource;

	return artist + " / " + album + " - " + title;
}
