#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "interaction_sdl.h"

#define FONT_FILE "/usr/local/lib/X11/fonts/bitstream-vera/Vera.ttf"
#define FONT_SIZE 8

int
InteractionSDL::init()
{
	SDL_Rect r;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_Init() failure: %s\n", SDL_GetError());
		return 0;
	}
	if (TTF_Init() < 0) {
		fprintf(stderr, "TTF_Init() failure: %s\n", SDL_GetError());
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

	font = TTF_OpenFont(FONT_FILE, FONT_SIZE);
	if (font == NULL) {
		fprintf(stderr, "TTF_OpenFont() failure: %s\n", SDL_GetError());
		return 0;
	}
	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

        if (SDL_MUSTLOCK(screen))
                SDL_LockSurface(screen);

	/* blank screen */
	clear(0, 0, getHeight(), getWidth());

	mouseX = -1; mouseY = -1;
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
	SDL_Surface* text;
	SDL_Rect rect;
	SDL_Color textcol = { 255, 255, 255 };

	text = TTF_RenderText_Solid(font, s, textcol); 
	rect.x = x; rect.y = y;
	rect.w = text->w; rect.h = text->h;
	SDL_BlitSurface(text, NULL, screen, &rect);
	SDL_FreeSurface(text);
}

void
InteractionSDL::gettextsize(const char* s, int* h, int* w)
{
	TTF_SizeText(font, s, w, h);
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
