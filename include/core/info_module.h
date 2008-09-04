#include "info.h"

#ifndef __INFO_MODULE_H__
#define __INFO_MODULE_H__

//! \brief Retrieve information on module files
class InfoModule : public Info {
public:
	InfoModule(Decoder* d) : Info(d) { }

	//! \brief Process an input file
	int load(const char* fname);
};

#endif /* __INFO_MODULE_H__ */
