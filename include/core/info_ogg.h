#include "info.h"

#ifndef __INFO_OGG_H__
#define __INFO_OGG_H__

//! \brief Retrieve information on Ogg files
class InfoOgg : public Info {
public:
	InfoOgg(Decoder* d) : Info(d) { }

	//! \brief Process an input file
	void load(const char* fname);
};

#endif /* __INFO_OGG_H__ */
