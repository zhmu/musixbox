#include <mad.h>
#include "output.h"
#include "visualize.h"

#ifndef __DECODE_H__
#define __DECODE_H__

class Decoder {
public:
	Decoder() {
		output = NULL; visualizer = NULL;
	}
	virtual int open(const char*) = 0;
	virtual int close() { };
	virtual int run() = 0;

	inline void setOutput(Output* o) { output = o; }
	inline void setVisualizer(Visualizer* v) { visualizer = v; }

protected:
	Output* output;
	Visualizer* visualizer;
	
};

#endif /* __DECODE_H__ */
