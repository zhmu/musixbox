#include "misc/playlist.h"

using namespace std;

Playlist::~Playlist()
{
	for (vector<PlaylistItem*>::iterator it = items.begin(); it != items.end(); it++) {
		delete *it;
	}
}

void
Playlist::addItem(PlaylistItem* it)
{
	items.insert(items.end(), it);
}

void
Playlist::removeItem(unsigned int num)
{
	/* Do not attempt to delete if there are no items */
	if (num >= items.size())
		return;

	/* Get rid of the item itself, and walk the structure to do the same */
	delete items[num];
	for (vector<PlaylistItem*>::iterator ite = items.begin(); ite != items.end(); ite++) {
		if (!num--) {
			items.erase(ite);
			return;
		}
	}
}

void
Playlist::removeItem(string resource)
{
	int num = 0;
	for (vector<PlaylistItem*>::iterator ite = items.begin(); ite != items.end(); ite++, num++) {
		if ((*ite)->getResource() == resource) {
			delete items[num];
			items.erase(ite);
			return;
		}
	}
}

void
Playlist::clear()
{
	items.clear();
	currentPlayItem = 0;
}

string
Playlist::getNextResource()
{
	if (currentPlayItem + 1 == items.size())
		return "";

	return items[++currentPlayItem]->getResource();
}

string
Playlist::getCurrentResource()
{
	if (currentPlayItem == items.size())
		return "";

	return items[currentPlayItem]->getResource();
}
