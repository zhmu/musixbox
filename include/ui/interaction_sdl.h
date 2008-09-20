#include <SDL/SDL.h>
#include "interaction.h"

#ifndef __INTERACTION_SDL_H__
#define __INTERACTION_SDL_H__

//! \brief Provides interaction using SDL and 
class InteractionSDL : public Interaction {
public:
	//! \brief Initialize the interaction provider
	InteractionSDL();

	//! \brief Deinitialize the interaction provider
	virtual ~InteractionSDL();

	//! \brief Returns the height of the SDL window
	inline unsigned int getHeight() { return 64; }

	//! \brief Returns the width of the SDL window
	inline unsigned int getWidth() { return 128; }
	
	//! \brief Used to handle interactions
	void yield();

	void clear(unsigned int x, unsigned int y, unsigned int h, unsigned int w);
	void putpixel(unsigned int x, unsigned int y, unsigned int c);

private:
	SDL_Surface* screen;
};

#endif /* __INTERACTION_SDL_H__ */
