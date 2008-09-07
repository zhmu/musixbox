#include "misc/playlist.h"

using namespace std;

Playlist::~Playlist()
{
	for (vector<PlaylistItem*>::iterator it = items.begin(); it != items.end(); it++) {
		delete *it;
	}
	items.clear();
}

void
Playlist::addItem(PlaylistItem* it)
{
}

void
Playlist::removeItem(PlaylistItem* it)
{
}

int
Playlist::getCurrentPlayItem()
{
}
