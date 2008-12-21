#include <string>
#include "config.h"
#include "core/folder.h"
#include "core/folder_fs.h"
#if defined(WITH_CURL) && defined(WITH_XML)
#include "core/folder_remote.h"
#endif
#include "core/folderfactory.h"

using namespace std;

void
FolderFactory::construct(string resource, Folder** folder)
{
#if defined(WITH_CURL) && defined(WITH_XML)
	/*
	 * If we find :// in the filename and CURL is available, assume we
	 * are playing a stream.
	 */
	if (resource.find("://") != string::npos) {
		*folder = new FolderRemote(resource);
	} else
#endif
		*folder = new FolderFS(resource);
}
