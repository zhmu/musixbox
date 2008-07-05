#include "interaction_chain.h"

void
InteractionChain::add(Interaction* i)
{
	provider.push_back(i);
}

int
InteractionChain::init()
{
	for (int i = 0; i < provider.size(); i++)
		if (!provider[i]->init())
			return 0;
	return 1;
}

void
InteractionChain::yield()
{
	for (int i = 0; i < provider.size(); i++) {
		provider[i]->yield();
		if (provider[i]->mustTerminate())
			terminating++;
	}
}
	
void
InteractionChain::done()
{
	for (int i = 0; i < provider.size(); i++)
		provider[i]->done();
}

int
InteractionChain::getCoordinates(int* x, int* y)
{
	for (int i = 0; i < provider.size(); i++) {
		if (provider[i]->getCoordinates(x, y))
			return 1;
	}
	return 0;
}

void
InteractionChain::clear(int x, int y, int h, int w)
{
	for (int i = 0; i < provider.size(); i++)
		provider[i]->clear(x, y, h, w);
}

void
InteractionChain::putpixel(int x, int y, int c)
{
	for (int i = 0; i < provider.size(); i++)
		provider[i]->putpixel(x, y, c);
}

void
InteractionChain::puttext(int x, int y, const char* s)
{
	for (int i = 0; i < provider.size(); i++)
		provider[i]->puttext(x, y, s);
}

void
InteractionChain::gettextsize(const char* s, int* h, int* w)
{
	provider[0]->gettextsize(s, h, w);
}
