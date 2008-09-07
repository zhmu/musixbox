#include "menuplaylist.h"

std::string
MenuPlaylist::getItem(unsigned int num)
{
	if (playlist->getCurrentPlayItem() == num)
		return " * " + playlist->getItems()[num]->getDisplayText();
	else
		return "   " + playlist->getItems()[num]->getDisplayText();
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
