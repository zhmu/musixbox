#include "config.h"
#include <SDL/SDL.h>
#ifdef WITH_SDLGFX
#include <SDL/SDL_rotozoom.h>
#endif
#include "core/exceptions.h"
#include "ui/interaction_sdl.h"

using namespace std;

InteractionSDL::InteractionSDL(unsigned int scale_factor)
	: Interaction()
{
#ifndef WITH_SDLGFX
	if (scale_factor != 1)
		throw InteractionException(string("SDL_gfx library not available - cannot scale"));
#endif
	scale = scale_factor;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw InteractionException(string("SDL_Init() failure: ") + SDL_GetError());

	screen = SDL_SetVideoMode(getWidth() * getScale(),
	                          getHeight() * getScale(),
	                          32, SDL_SWSURFACE);
	if (screen == NULL)
		throw InteractionException(string("SDL_SetVideoMode() failure: ") + SDL_GetError());
	if (screen->format->BytesPerPixel != 4)
		throw InteractionException(string("FIXME: SDL non-32-bit pixels are not yet supported"));

	display = SDL_CreateRGBSurface(SDL_SWSURFACE, getWidth(), getHeight(), 32, 0, 0, 0, 0);
	if (display == NULL)
		throw InteractionException(string("SDL_CreateRGBSurface() failure: ") + SDL_GetError());
        if (SDL_MUSTLOCK(screen))
                SDL_LockSurface(screen);

	/* blank screen */
	clear(0, 0, getHeight(), getWidth());
}
	
InteractionSDL::~InteractionSDL()
{
	SDL_FreeSurface(display);
	SDL_Quit();
}

void
InteractionSDL::yield()
{
	SDL_Event event;

#ifdef WITH_SDLGFX
	if (getScale() == 1) {
#endif /* WITH_SDLGFX */
		SDL_BlitSurface(display, NULL, screen, NULL);
#ifdef WITH_SDLGFX
	} else {
		SDL_Surface* surf = rotozoomSurfaceXY(display, 0, getScale(), getScale(), 0);
		if (surf != NULL) {
			SDL_BlitSurface(surf, NULL, screen, NULL);
			SDL_FreeSurface(surf);
		}
	}
#endif /* WITH_SDLGFX */
	SDL_Flip(screen);

	while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_MOUSEBUTTONDOWN:
			               setInteraction(event.button.x / getScale(),
					              event.button.y / getScale(),
						(event.button.button == SDL_BUTTON_LEFT) ? INTERACTION_TYPE_NORMAL : INTERACTION_TYPE_ALTERNATIVE);
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
InteractionSDL::putpixel(unsigned int x, unsigned int y, unsigned int c)
{
	Uint32 color;
	Uint32* p;

	if (c)
		 color = SDL_MapRGB(display->format, 255, 255, 255);
	else
		 color = SDL_MapRGB(display->format, 100, 100, 100);

	if (x >= getWidth() || y >= getHeight())
		return;

	p = (Uint32*)display->pixels + y * display->pitch/4 + x;
	*p = color;
}

void
InteractionSDL::clear(unsigned int x, unsigned int y, unsigned int h, unsigned int w)
{
	SDL_Rect r;
	r.x = x; r.y = y;
	r.h = h; r.w = w;
	SDL_FillRect(display, &r, SDL_MapRGB(display->format, 100, 100, 100));
}
