#include "info.h"

#ifndef __INFO_MP3_ID3LIB_H__
#define __INFO_MP3_ID3LIB_H__

//! \brief Retrieve information on MP3 files using id3lib
class InfoMP3_ID3Lib: public Info {
public:
	InfoMP3_ID3Lib(Decoder* d) : Info(d) { }

	/*! \brief Load information from a resource
	 *  \param res Resource to process
	 */
	void load(std::string res);

private:
	char* safe_strdup(const char* s);
};

#endif /* __INFO_MP3_ID3LIB_H__ */
