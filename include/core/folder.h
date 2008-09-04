#include <map>
#include <string>
#include <vector>

#ifndef __FOLDER_H__
#define __FOLDER_H__

//! \brief Abstract class to provide walking through directory trees
class Folder {
public:
	/*! \brief Constructs a directory walking object
	 *  \param r Root directory
	 */
	Folder(std::string r);

	//! \brief Destructs the directory walking object
	inline virtual ~Folder() { }

	//! \brief Goes one level up
	void goUp();

	/*! \brief Selects a child directory to visit
	 *  \param dir Child directory
	 */
	void select(std::string dir);

	//! \brief Retrieve the current path
	std::string getPath() { return current; }

	//! \brief Retrieve full path of an entry
	std::string getFullPath(std::string entry);

	/*! \brief Check whether an entry is a folder
	 *  \param entry Entry to check
	 *  \returns true if the entry is a folder
	 */
	virtual bool isFolder(std::string entry) = 0;

	//! \brief Retrieve a vector containing current directory contents
	inline std::vector<std::string> getEntries() { return entries; }

	//! \brief Updates current directory contents
	void rehash();

protected:
	//! \brief Called to fill the current directory entries list
	virtual void retrieveEntries() = 0;

	//! \brief Root directory, won't go below this one
	std::string root;

	//! \brief Current directory being browsed
	std::string current;

	//! \brief Entries in the current directory
	std::vector<std::string> entries;
};

#endif /* __FOLDER_H__ */
