#include <SDL/SDL.h>
#include "interaction.h"

#ifndef __INTERACTION_SDL_H__
#define __INTERACTION_SDL_H__

//! \brief Provides interaction using SDL and 
class InteractionSDL : public Interaction {
public:
	/*! \brief Initialize the interaction provider
	 *  \param scale_factor Scale factor to use
	 */
	InteractionSDL(unsigned int scale_factor = 1);

	//! \brief Deinitialize the interaction provider
	virtual ~InteractionSDL();

	//! \brief Returns the height of the SDL window
	inline unsigned int getHeight() { return 64; }

	//! \brief Returns the width of the SDL window
	inline unsigned int getWidth() { return 128; }

	//! \brief Scale factor
	inline unsigned int getScale() { return scale; }
	
	//! \brief Used to handle interactions
	void yield();

	void clear(unsigned int x, unsigned int y, unsigned int h, unsigned int w);
	void putpixel(unsigned int x, unsigned int y, unsigned int c);

private:
	//! \brief The surface visible to the user
	SDL_Surface* screen;

	//! \brief Our internal service
	SDL_Surface* display;

	//! \brief Scale factor
	unsigned int scale;
};

#endif /* __INTERACTION_SDL_H__ */
