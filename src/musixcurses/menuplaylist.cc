#include "menuplaylist.h"

std::string
MenuPlaylist::getItem(unsigned int num)
{
	char modifier[4] = "   ";

	if (playlist->getCurrentPlayItem() == num)
		modifier[1] = '*';
	if (playlist->getNextPlayItem() == num)
		modifier[0] = '>';
	return modifier + playlist->getItems()[num]->getDisplayText();
}

std::string
MenuPlaylist::getCompareItem(unsigned int num)
{
	return playlist->getItems()[num]->getDisplayText();
}

unsigned int
MenuPlaylist::getNumItems()
{
	return playlist->getItems().size();
}
