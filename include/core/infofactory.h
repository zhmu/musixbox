#include <list>
#include <string>
#include "input.h"
#include "info.h"
#include "output.h"
#include "visualize.h"

#ifndef __INFOFACTORY_H__
#define __INFOFACTORY_H__

//! \brief Static class that aids in obtaining resource information
class InfoFactory {
public:
	/*! \brief Constructs an information object based on a resource
	 *  \param resource Resource to use
	 *  \param info Resulting info object
	 */
	static void construct(std::string resource, Info** info);
};

#endif /* __INFOFACTORY_H__ */
