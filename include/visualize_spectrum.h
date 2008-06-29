#include "visualize.h"

#ifndef __VISUALIZE_SPECTRUM_H__
#define __VISUALIZE_SPECTRUM_H__

class SpectrumVisualizer : public Visualizer {
public:
	int init();
	void update(const char* input, unsigned int num);
	void done();
};

#endif /* __VISUALIZE_SPECTRUM_H__ */
