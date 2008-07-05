#include <SDL/SDL.h>
#include "interaction.h"

#ifndef __INTERACTION_SDL_H__
#define __INTERACTION_SDL_H__

//! \brief Provides interaction using SDL and 
class InteractionSDL : public Interaction {
public:
	//! \brief Initialize SDL interaction provider
	int init();

	//! \brief Deinitialize SDL interaction provider
	void done();

	//! \brief Returns the height of the SDL window
	inline int getHeight() { return 64; }

	//! \brief Returns the width of the SDL window
	inline int getWidth() { return 128; }
	
	//! \brief Used to handle interactions
	void yield();

	void clear(int x, int y, int h, int w);
	void putpixel(int x, int y, int c);

private:
	SDL_Surface* screen;

	int mouseX, mouseY;
};

#endif /* __INTERACTION_SDL_H__ */
