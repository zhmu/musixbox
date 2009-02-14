#include <unistd.h>
#include "ui/interaction_chain.h"

using namespace std;

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
InteractionChain::getInteraction(unsigned int* x, unsigned int* y, unsigned int* type)
{
	for (unsigned int i = 0; i < provider.size(); i++) {
		if (provider[i]->getInteraction(x, y, type))
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
InteractionChain::puttext(unsigned int x, unsigned int y, const char* s, FONT* font)
{
	for (unsigned int i = 0; i < provider.size(); i++)
		provider[i]->puttext(x, y, s, font);
}

void
InteractionChain::gettextsize(const char* s, unsigned int* h, unsigned int* w, FONT* font)
{
	provider[0]->gettextsize(s, h, w, font);
}

void
InteractionChain::flushInteraction()
{
	for (unsigned int i = 0; i < provider.size(); i++)
		provider[i]->flushInteraction();
}
