#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "exceptions.h"
#include "folder_fs.h"

FolderFS::FolderFS(std::string r) :
	Folder(r)
{
	/*
	 * Every derivative class has to call the rehash() function here - refer
	 * to folder.cc for an explanation why this is.
	 */
	rehash();
}

void
FolderFS::retrieveEntries()
{
	DIR* dir;
	struct dirent* dent;

	dir = opendir(current.c_str());
	if (dir == NULL)
		throw FolderException(std::string("FolderFS: unable to open folder ") + current);

	while((dent = readdir(dir)) != NULL) {
		// Never show the current directory '.'
		if (!strcmp(dent->d_name, "."))
			continue;
		// Don't allow travelling below the root path
		if (!strcmp(dent->d_name, "..") && current == root)
			continue;
		entries.push_back(dent->d_name);
	}
	closedir(dir);
}

bool
FolderFS::isFolder(std::string entry)
{
	struct stat fs;

	if (stat(getFullPath(entry).c_str(), &fs) < 0)
		throw FolderException(std::string("FolderFS: can't stat ") + getFullPath(entry));

	return S_ISDIR(fs.st_mode) ? true : false;
}
