#include "info.h"

#ifndef __INFO_MODULE_H__
#define __INFO_MODULE_H__

//! \brief Retrieve information on module files
class InfoModule : public Info {
public:
	InfoModule(Decoder* d) : Info(d) { }

	/*! \brief Load information from a resource
	 *  \param res Resource to process
	 */
	void load(std::string res);
};

#endif /* __INFO_MODULE_H__ */
