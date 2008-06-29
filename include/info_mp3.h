#include "info.h"

#ifndef __INFO_MP3_H__
#define __INFO_MP3_H__

//! \brief Retrieve information on MP3 files
class InfoMP3 : public Info {
public:
	//! \brief Process an input file
	int load(const char* fname);
};

#endif /* __INFO_MP3_H__ */
