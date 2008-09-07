#include "info.h"

#ifndef __INFO_MP3_H__
#define __INFO_MP3_H__

//! \brief Retrieve information on MP3 files
class InfoMP3 : public Info {
public:
	InfoMP3(Decoder* d) : Info(d) { }

	/*! \brief Load information from a resource
	 *  \param res Resource to process
	 */
	void load(std::string res);
};

#endif /* __INFO_MP3_H__ */
