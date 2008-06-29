#include "output.h"

#ifndef __OUTPUT_NULL_H__
#define __OUTPUT_NULL_H__

class OutputNULL : public Output {
public:
	inline void play(char* buf, size_t len) { }
};

#endif /* __OUTPUT_NULL_H__ */
