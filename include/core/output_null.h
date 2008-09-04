#include "output.h"

#ifndef __OUTPUT_NULL_H__
#define __OUTPUT_NULL_H__

//! \brief Dummy output plugin that does not make noise
class OutputNull : public Output {
public:
	//! \brief Discards the output
	inline void play(char* buf, size_t len) { }
};

#endif /* __OUTPUT_NULL_H__ */
