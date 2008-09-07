#include "info.h"

#ifndef __INFO_SID_H__
#define __INFO_SID_H__

//! \brief Retrieve information on SID files
class InfoSID : public Info {
public:
	InfoSID(Decoder* d) : Info(d) { }

	//! \brief Process an input file
	void load(const char* fname);
};

#endif /* __INFO_SID_H__ */
