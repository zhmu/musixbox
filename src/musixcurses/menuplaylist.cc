#include "menuplaylist.h"

std::string
MenuPlaylist::getItem(unsigned int num)
{
	if (playlist->getCurrentPlayItem() == num)
		return " * " + playlist->getItems()[num]->getResource();
	else
		return "   " + playlist->getItems()[num]->getResource();
}

unsigned int
MenuPlaylist::getNumItems()
{
	return playlist->getItems().size();
}
