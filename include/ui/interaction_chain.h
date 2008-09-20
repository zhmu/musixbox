#include <vector>
#include "interaction.h"

#ifndef __INTERACTION_CHAIN_H__
#define __INTERACTION_CHAIN_H__

//! \brief Provides interaction using a chain of other providers
class InteractionChain : public Interaction {
public:
	//! \brief Destructs the chain and all providers in it
	virtual ~InteractionChain();

	//! \brief Returns the height of the chain window
	inline unsigned int getHeight() { return provider[0]->getHeight(); }

	//! \brief Returns the width of the Chain window
	inline unsigned int getWidth() { return provider[0]->getWidth(); }

	//! \brief Returns the size of a text string
	inline unsigned int getTextHeight() { return provider[0]->getTextHeight(); }

	//! \brief Returns the number of interaction providers registered
	inline unsigned int getNumProviders() { return provider.size(); }
	
	//! \brief Used to handle interactions
	void yield();

	void add(Interaction* i);

	void clear(unsigned int x, unsigned int y, unsigned int h, unsigned int w);
	void putpixel(unsigned int x, unsigned int y, unsigned int c);
	void puttext(unsigned int x, unsigned int y, const char* s);
	void gettextsize(const char* s, unsigned int* h, unsigned int* w);
	bool getInteraction(unsigned int* x, unsigned int* y, unsigned int* type);
	void flushInteraction();

private:
	std::vector<Interaction*> provider;
};

#endif /* __INTERACTION_CHAIN_H__ */
