#include <map>
#include <string>
#include "folder.h"

#ifndef __FOLDER_REMOTE_H__
#define __FOLDER_REMOTE_H__

//! \brief Stores information per folder
class RemoteFolderInfo {
public:
	/*! \brief Constructs a new folder information object
	 *  \param location Location of the content
	 *  \param folder Is this a folder
	 */
	inline RemoteFolderInfo(std::string location, bool folder) {
		this->location = location; this->folder = folder;
	}

	//! \brief Retrieve target location
	inline std::string getLocation() { return location; }

	//! \brief Is this a folder?
	inline bool isFolder() { return folder; }

private:
	//! \brief Location of the content
	std::string location;

	//! \brief Is this a folder?
	bool folder;
};

//! \brief Class to provide walking through a remote folder
class FolderRemote : public Folder {
public:
	/*! \brief Constructs a directory walking object
	 *  \param r Root directory
	 */
	FolderRemote(std::string r);

	//! \brief Destructs the directory walker
	~FolderRemote();

	/*! \brief Check whether an entry is a folder
	 *  \param entry Entry to check
	 *  \returns true if the entry is a folder
	 */
	bool isFolder(std::string entry);

	//! \brief Goes one level up
	void goUp();

	/*! \brief Selects a child directory to visit
	 *  \param dir Child directory
	 */
	virtual void select(std::string dir);

	//! \brief Retrieve full path of an entry
	virtual std::string getFullPath(std::string entry);

protected:
	//! \brief Called to fill the current directory entries list
	virtual void retrieveEntries();

	//! \brief Clear the information list
	void clearInfo();

private:
	void fetchURL(std::string resource, std::string& s);

	//! \brief Map used to map the entry to subsequent information
	std::map<std::string, RemoteFolderInfo*> entry2info;

	//! \brief Location of the folder we came from
	std::string previous;
};

#endif /* __FOLDER_REMOTE_H__ */
