#include <vector>
#include "interaction.h"

#ifndef __INTERACTION_CHAIN_H__
#define __INTERACTION_CHAIN_H__

//! \brief Provides interaction using a chain of other providers
class InteractionChain : public Interaction {
public:
	//! \brief Initialize Chain interaction provider
	int init();

	//! \brief Deinitialize Chain interaction provider
	void done();

	//! \brief Returns the height of the chain window
	inline int getHeight() { return provider[0]->getHeight(); }

	//! \brief Returns the width of the Chain window
	inline int getWidth() { return provider[0]->getWidth(); }

	//! \brief Returns the size of a text string
	inline int getTextHeight() { return provider[0]->getTextHeight(); }

	//! \brief Returns the number of interaction providers registered
	inline int getNumProviders() { return provider.size(); }
	
	//! \brief Used to handle interactions
	void yield();

	void add(Interaction* i);

	void clear(int x, int y, int h, int w);
	void putpixel(int x, int y, int c);
	void puttext(int x, int y, const char* s);
	void gettextsize(const char* s, int* h, int* w);
	int getCoordinates(int* x, int* y);

private:
	std::vector<Interaction*> provider;
};

#endif /* __INTERACTION_CHAIN_H__ */
