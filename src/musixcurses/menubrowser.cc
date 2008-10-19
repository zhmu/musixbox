#include "menubrowser.h"

using namespace std;

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

void
MenuBrowser::attemptReturnPosition()
{
	/*
	 * Attempt to look up the visiting page number - if it's not
	 * there, just default to the top.
	 */
	map<string, unsigned int>::iterator it = cachedPositionMap.find(folder->getPath());

	if(it != cachedPositionMap.end()) {
		setSelectedItem(it->second);
	} else {
		setSelectedItem(0);
	}
}

void
MenuBrowser::storePosition()
{
	cachedPositionMap[folder->getPath()] = getSelectedItem();
}
