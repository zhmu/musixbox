#include "info.h"

#ifndef __INFO_SID_H__
#define __INFO_SID_H__

//! \brief Retrieve information on SID files
class InfoSID : public Info {
public:
	InfoSID(Decoder* d) : Info(d) { }

	/*! \brief Load information from a resource
	 *  \param res Resource to process
	 */
	void load(std::string res);
};

#endif /* __INFO_SID_H__ */
