#include <string>
#include "folder.h"

#ifndef __FOLDERFACTORY_H__
#define __FOLDERFACTORY_H__

//! \brief Static class that aids in constructing an appropriate folder object
class FolderFactory {
public:
	/*! \brief Constructs a folder for a given resource
	 *  \param resource Resource to use
	 *  \param output Output object to use
	 *  \param folder Resulting folder object
	 */
	static void construct(std::string resource, Folder** folder);
};

#endif /* __FOLDERFACTORY_H__ */
