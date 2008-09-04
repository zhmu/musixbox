#include "boxplayer.h"
#include "interface.h"

BoxPlayer::BoxPlayer(std::string resource, Output* o, Visualizer* v, Interface* i)
	: Player(resource, o, v)
{
	interface = i;
}

void
BoxPlayer::terminated()
{
	interface->trackDone();
}
