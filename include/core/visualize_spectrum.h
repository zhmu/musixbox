#include <fftw3.h>
#include "visualize.h"

#ifndef __VISUALIZE_SPECTRUM_H__
#define __VISUALIZE_SPECTRUM_H__

/*! \brief A spectrum analyzer visualizer
 *
 *  The Fast Fourier Transformation required is handeled by libfftw.
 */
class SpectrumVisualizer : public Visualizer {
public:
	//! \brief Construct a new FFT visualizer object
	SpectrumVisualizer();

	//! \brief Deinitialize the visualizer object
	~SpectrumVisualizer();

	//! \brief Update the spectrum analyzer based on current audio output
	void update(const char* audio, unsigned int num);

private:
	double* in;
	fftw_complex *out;
	fftw_plan p;
};

#endif /* __VISUALIZE_SPECTRUM_H__ */
