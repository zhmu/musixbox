#include "info.h"

#ifndef __INFO_OGG_H__
#define __INFO_OGG_H__

//! \brief Retrieve information on Ogg files
class InfoOgg : public Info {
public:
	InfoOgg(Decoder* d) : Info(d) { }

	/*! \brief Load information from a resource
	 *  \param res Resource to process
	 */
	void load(std::string res);
};

#endif /* __INFO_OGG_H__ */
