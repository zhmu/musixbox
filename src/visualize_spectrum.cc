#include <SDL/SDL.h>
#include <fftw3.h>
#include <math.h>
#include "visualize_spectrum.h"

#define MAX_VIS_SIZE	1152

int
SpectrumVisualizer::init()
{
	in = (double*)fftw_malloc(sizeof(double) * MAX_VIS_SIZE);
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * MAX_VIS_SIZE);

	p = fftw_plan_dft_r2c_1d(MAX_VIS_SIZE, in, out, 0);

	return 1;
}

void
SpectrumVisualizer::done()
{
	fftw_destroy_plan(p);
	fftw_free(in); fftw_free(out);
}
void
SpectrumVisualizer::update(const char* input, unsigned int num)
{
	unsigned int i;
	float max = 10;
	float min = 1000;

	if (num > MAX_VIS_SIZE)
		return;

	/*
	 * Pollute the input buffer by adding the left and right channel
	 * inputs together (divide by two afterwards)
	 */
	for (i = 0; i < num; i++) {
		in[i] = ((int)(input[i * 4  ] + (input[i * 4+1] << 8)) ^ 32768 +
			 (int)(input[i * 4+2] + (input[i * 4+3] << 8)) ^ 32768)
			/ 2;
	}

	fftw_execute(p);

	for (i = 0; i < num; i++) {
		/*
		 * out[i] is a complex number, so transform it to a float by
		 * taking the square of the real and imaginary parts.
		 */
		float r = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
		if (min > r) min = r;
		if (max < r) max = r;
		out[i][0] = r;
	}

#if 0
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

	SDL_Rect r;
	r.x = 0; r.y = 0;
	r.w = WIDTH; r.h =  HEIGHT;
	SDL_FillRect(screen, &r, 0);

	/* spectrum analyzer */
	for (i = 0; i < WIDTH; i++) {
		float f = out[i][0];
		if (min != max)
			j = (int)(HEIGHT - (f / ((max - min) / HEIGHT)));
		else
			j = HEIGHT;
		while (j < HEIGHT) {
			putpixel(i, j++, 100, 100, 100);
		}
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
	SDL_Flip(screen);
#endif
}
