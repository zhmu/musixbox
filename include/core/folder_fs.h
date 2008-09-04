#include <string>
#include "folder.h"

#ifndef __FOLDER_FS_H__
#define __FOLDER_FS_H__

//! \brief Class to provide walking through local folder on a filesystem
class FolderFS : public Folder {
public:
	/*! \brief Constructs a directory walking object
	 *  \param r Root directory
	 */
	FolderFS(std::string r);

	/*! \brief Check whether an entry is a folder
	 *  \param entry Entry to check
	 *  \returns true if the entry is a folder
	 */
	virtual bool isFolder(std::string entry);

protected:
	//! \brief Called to fill the current directory entries list
	virtual void retrieveEntries();
};

#endif /* __FOLDER_FS_H__ */
