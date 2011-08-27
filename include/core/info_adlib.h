#include "info.h"

#ifndef __INFO_ADLIB_H__
#define __INFO_ADLIB_H__

//! \brief Retrieve information on AdLib files
class InfoAdLib : public Info {
public:
	InfoAdLib(Decoder* d) : Info(d) { }

	/*! \brief Load information from a resource
	 *  \param res Resource to process
	 */
	void load(std::string res);
};

#endif /* __INFO_ADLIB_H__ */
