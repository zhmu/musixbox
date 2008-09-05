#include "curseplayer.h"
#include "interface.h"

CursePlayer::CursePlayer(std::string resource, Output* o, Interface* i)
	: Player(resource, o, NULL)
{
	interface = i;
}

void
CursePlayer::terminated()
{
	interface->trackDone();
}
