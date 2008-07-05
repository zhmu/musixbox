#include <SDL/SDL.h>
#include "font.h"
#include "interaction_sdl.h"

int
InteractionSDL::init()
{
	SDL_Rect r;

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

	mouseX = -1; mouseY = -1;
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
			               mouseX = event.button.x;
			               mouseY = event.button.y;
			               break;
			case SDL_KEYDOWN:
			               if (event.key.keysym.sym == SDLK_ESCAPE)
			                 terminating++;
			               break;
			case SDL_QUIT: terminating++;
			               break;
		}
	}

	/*
	 * Do not clutter the CPU - a granularity of 100ms is enough.
	 */
	SDL_Delay(100);
}
	
void
InteractionSDL::done()
{
	SDL_Quit();
}


void
InteractionSDL::putpixel(int x, int y, int c)
{
	Uint32 color;
	Uint32* p;

	if (c)
		 color = SDL_MapRGB(screen->format, 255, 255, 255);
	else
		 color = SDL_MapRGB(screen->format, 100, 100, 100);

	if (x < 0 || y < 0 || x >= getWidth() || y >= getHeight())
		return;

	p = (Uint32*)screen->pixels + y * screen->pitch/4 + x;
	*p = color;
}

void
InteractionSDL::puttext(int x, int y, const char* s)
{
	for (; *s; s++) {
		struct CHARACTER* c = &theFont[*s];
		for (int i = 0; i < c->width; i++) {
			for (int j = 0; j < 8 /*c->height*/; j++) {
				unsigned char d = c->data[i * ((j / 8) + 1)];
				if (d & (1 << j))
					putpixel(x + i, y + j + (c->height - c->yshift), 1);
			}
		}
		x += c->advance_x;
	}
}

void
InteractionSDL::gettextsize(const char* s, int* h, int* w)
{
}

int
InteractionSDL::getCoordinates(int* x, int* y)
{
	if (mouseX == -1 && mouseY == -1)
		return 0;
	*x = mouseX; *y = mouseY;
	 mouseX = -1; mouseY = -1;
	return 1;
}

void
InteractionSDL::clear(int x, int y, int h, int w)
{
	SDL_Rect r;
	r.x = x; r.y = y;
	r.h = h; r.w = w;
	SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 100, 100, 100));
}
