#include <string>
#include "folder.h"
#include "folder_fs.h"
#include "folderfactory.h"

void
FolderFactory::construct(std::string resource, Folder** folder)
{
	*folder = new FolderFS(resource);
}
