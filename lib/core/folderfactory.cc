#include <string>
#include "folder.h"
#include "folder_fs.h"
#include "folderfactory.h"

using namespace std;

void
FolderFactory::construct(string resource, Folder** folder)
{
	*folder = new FolderFS(resource);
}
