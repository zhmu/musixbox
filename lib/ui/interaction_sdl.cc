#include <SDL/SDL.h>
#include "core/exceptions.h"
#include "ui/interaction_sdl.h"

InteractionSDL::InteractionSDL() : Interaction()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw InteractionException(std::string("SDL_Init() failure: ") + SDL_GetError());

	screen = SDL_SetVideoMode(getWidth(), getHeight(), 32, SDL_SWSURFACE);
	if (screen == NULL)
		throw InteractionException(std::string("SDL_SetVideoMode() failure: ") + SDL_GetError());
	if (screen->format->BytesPerPixel != 4)
		throw InteractionException(std::string("FIXME: SDL non-32-bit pixels are not yet supported"));

        if (SDL_MUSTLOCK(screen))
                SDL_LockSurface(screen);

	/* blank screen */
	clear(0, 0, getHeight(), getWidth());
}
	
InteractionSDL::~InteractionSDL()
{
	SDL_Quit();
}


void
InteractionSDL::yield()
{
	SDL_Event event;

	SDL_UpdateRect(screen, 0, 0, 0, 0);

	while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_MOUSEBUTTONDOWN:
			               setInteraction(event.button.x, event.button.y,
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
