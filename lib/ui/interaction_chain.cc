#include "ui/interaction_chain.h"

InteractionChain::~InteractionChain()
{
	for (unsigned int i = 0; i < provider.size(); i++) {
		delete provider[i];
	}
}

void
InteractionChain::add(Interaction* i)
{
	provider.push_back(i);
}

void
InteractionChain::yield()
{
	for (unsigned int i = 0; i < provider.size(); i++) {
		provider[i]->yield();
		if (provider[i]->mustTerminate())
			requestTermination();
	}

	/*
	 * Delay for a far bit of time to give the OS the chance to do
	 * something else...
	 */
	usleep(250);
}

bool
InteractionChain::getCoordinates(unsigned int* x, unsigned int* y)
{
	for (unsigned int i = 0; i < provider.size(); i++) {
		if (provider[i]->getCoordinates(x, y))
			return true;
	}
	return false;
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

void
InteractionChain::flushCoordinates()
{
	for (unsigned int i = 0; i < provider.size(); i++)
		provider[i]->flushCoordinates();
}
