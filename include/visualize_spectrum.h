#include "visualize.h"

#ifndef __VISUALIZE_SPECTRUM_H__
#define __VISUALIZE_SPECTRUM_H__

/*! \brief A spectrum analyzer visualizer
 *
 *  The Fast Fourier Transformation required is handeled by libfftw.
 */
class SpectrumVisualizer : public Visualizer {
public:
	//! \brief Initialize the spectrum analyzer
	int init();

	//! \brief Update the spectrum analyzer based on current audio output
	void update(const char* audio, unsigned int num);

	//! \brief Deinitialize the spectrum analyzer
	void done();
};

#endif /* __VISUALIZE_SPECTRUM_H__ */
