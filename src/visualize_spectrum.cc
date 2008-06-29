#include <SDL/SDL.h>
#include <fftw3.h>
#include <math.h>
#include "visualize_spectrum.h"

#define WIDTH		640
#define HEIGHT		480

#define MAX_VIS_SIZE	1152

double* in;
fftw_complex *out;
fftw_plan p;

SDL_Surface* screen;

int
SpectrumVisualizer::init()
{
	in = (double*)fftw_malloc(sizeof(double) * MAX_VIS_SIZE);
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * MAX_VIS_SIZE);

	p = fftw_plan_dft_r2c_1d(MAX_VIS_SIZE, in, out, 0);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_Init() failure: %s\n", SDL_GetError());
		return 0;
	}

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (screen == NULL) {
		fprintf(stderr, "SDL_SetVideoMode() failure: %s\n", SDL_GetError());
		return 0;
	}
	if (screen->format->BytesPerPixel != 4) {
		printf("TODO\n");
		return 0;
	}
	return 1;
}

void
SpectrumVisualizer::done()
{
	fftw_destroy_plan(p);
	fftw_free(in); fftw_free(out);
	SDL_Quit();
}

void
putpixel(int x, int y, int r, int g, int b)
{
	Uint32 color = SDL_MapRGB(screen->format, r, g, b);
	Uint32* p;

	if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
		return;

	p = (Uint32*)screen->pixels + y * screen->pitch/4 + x;
	*p = color;
}

void
SpectrumVisualizer::update(const char* input, unsigned int num)
{
	int i, j;
	float max = 10;
	float min = 1000;

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
//		printf("%f => %f\n", in[i], sqrt(r));
	}

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
		//printf("%f ", out[i][0]);
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
	SDL_Flip(screen);
}
