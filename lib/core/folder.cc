#include <algorithm>
#include "folder.h"

Folder::Folder(std::string r)
{
	root = r; current = r;
	/*
	 * Note that we can *NOT* call rehash() here! The reason this fails, is
	 * because rehash() calls retrieveEntries(), which is a pure virtual.
	 *
	 * However, since we are still in our constructor, C++ won't know that
	 * it has to call the derived class' function, and thus calls the pure
	 * virtual.
	 *
	 * The behaviour of calling virtual functions in the constructor is
	 * considered undefined in the C++ specification...
	 */
}

void
Folder::goUp()
{
	current = std::string(current.begin(), current.begin() + current.find_last_of("/"));
	rehash();
}

void
Folder::select(std::string dir)
{
	current = current + "/" + dir;
	rehash();
}

void
Folder::rehash()
{
	entries.clear();
	retrieveEntries();
	std::sort(entries.begin(), entries.end());
}

std::string
Folder::getFullPath(std::string entry)
{
	return current + "/" + entry;
}
