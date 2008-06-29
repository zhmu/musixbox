#include <ao/ao.h>
#include "output.h"

#ifndef __OUTPUT_AO_H__
#define __OUTPUT_AO_H__

class OutputAO : public Output {
public:
	OutputAO() : Output() {
		output_dev = NULL;
	}
	int init();
	int done();
	void play(char* buf, size_t len);

private:
	ao_device* output_dev;
};

#endif /* __OUTPUT_AO_H__ */
