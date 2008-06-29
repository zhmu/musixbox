#include <mad.h>
#include "input.h"
#include "output.h"
#include "visualize.h"

#ifndef __DECODE_H__
#define __DECODE_H__

#define DECODER_OUTBUF_SIZE	8192

class Decoder {
public:
	Decoder();
	~Decoder();

	virtual int run() = 0;

	inline void setInput(Input* i) { input = i; }
	inline void setOutput(Output* o) { output = o; }
	inline void setVisualizer(Visualizer* v) { visualizer = v; }

protected:
	Input* input;
	Output* output;
	Visualizer* visualizer;

	char* out_buffer;
};

#endif /* __DECODE_H__ */
