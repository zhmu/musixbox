#include "interaction_chain.h"

void
InteractionChain::add(Interaction* i)
{
	provider.push_back(i);
}

int
InteractionChain::init()
{
	for (unsigned int i = 0; i < provider.size(); i++)
		if (!provider[i]->init())
			return 0;
	return 1;
}

void
InteractionChain::yield()
{
	for (unsigned int i = 0; i < provider.size(); i++) {
		provider[i]->yield();
		if (provider[i]->mustTerminate())
			terminating++;
	}
}
	
void
InteractionChain::done()
{
	for (unsigned int i = 0; i < provider.size(); i++)
		provider[i]->done();
}

int
InteractionChain::getCoordinates(unsigned int* x, unsigned int* y)
{
	for (unsigned int i = 0; i < provider.size(); i++) {
		if (provider[i]->getCoordinates(x, y))
			return 1;
	}
	return 0;
}

void
InteractionChain::clear(unsigned int x, unsigned int y, unsigned int h, unsigned int w)
{
	for (unsigned int i = 0; i < provider.size(); i++)
		provider[i]->clear(x, y, h, w);
}

void
InteractionChain::putpixel(unsigned int x, unsigned int y, unsigned int c)
{
	for (unsigned int i = 0; i < provider.size(); i++)
		provider[i]->putpixel(x, y, c);
}

void
InteractionChain::puttext(unsigned int x, unsigned int y, const char* s)
{
	for (unsigned int i = 0; i < provider.size(); i++)
		provider[i]->puttext(x, y, s);
}

void
InteractionChain::gettextsize(const char* s, unsigned int* h, unsigned int* w)
{
	provider[0]->gettextsize(s, h, w);
}

bool
InteractionChain::isScrollingOK()
{
	for (unsigned int i = 0; i < provider.size(); i++)
		if (!provider[i]->isScrollingOK())
			return false;
	return true;
}
