#include "menubrowser.h"

std::string
MenuBrowser::getItem(unsigned int num)
{
	return folder->getEntries()[num];
}

unsigned int
MenuBrowser::getNumItems()
{
	return folder->getEntries().size();
}
