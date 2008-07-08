#include <SDL/SDL.h>
#include "font.h"
#include "interaction_sdl.h"

int
InteractionSDL::init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_Init() failure: %s\n", SDL_GetError());
		return 0;
	}

	screen = SDL_SetVideoMode(getWidth(), getHeight(), 32, SDL_SWSURFACE);
	if (screen == NULL) {
		fprintf(stderr, "SDL_SetVideoMode() failure: %s\n", SDL_GetError());
		return 0;
	}
	if (screen->format->BytesPerPixel != 4) {
		printf("TODO\n");
	}

        if (SDL_MUSTLOCK(screen))
                SDL_LockSurface(screen);

	/* blank screen */
	clear(0, 0, getHeight(), getWidth());
	return 1;
}

void
InteractionSDL::yield()
{
	SDL_Event event;

	SDL_UpdateRect(screen, 0, 0, 0, 0);

	while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_MOUSEBUTTONDOWN:
			               setCoordinates(event.button.x, event.button.y);
			               break;
			case SDL_KEYDOWN:
			               if (event.key.keysym.sym == SDLK_ESCAPE)
			                 requestTermination();
			               break;
			case SDL_QUIT: requestTermination();
			               break;
		}
	}
}
	
void
InteractionSDL::done()
{
	SDL_Quit();
}


void
InteractionSDL::putpixel(unsigned int x, unsigned int y, unsigned int c)
{
	Uint32 color;
	Uint32* p;

	if (c)
		 color = SDL_MapRGB(screen->format, 255, 255, 255);
	else
		 color = SDL_MapRGB(screen->format, 100, 100, 100);

	if (x >= getWidth() || y >= getHeight())
		return;

	p = (Uint32*)screen->pixels + y * screen->pitch/4 + x;
	*p = color;
}

void
InteractionSDL::clear(unsigned int x, unsigned int y, unsigned int h, unsigned int w)
{
	SDL_Rect r;
	r.x = x; r.y = y;
	r.h = h; r.w = w;
	SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 100, 100, 100));
}
