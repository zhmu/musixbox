#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "decode_adlib.h"
#include "info_adlib.h"

using namespace std;

void
InfoAdLib::load(string res)
{
	CPlayer* adplay = (reinterpret_cast<DecoderAdLib*> (decoder))->getPlayer();

	if (!adplay->gettitle().empty())
		title = strdup(adplay->gettitle().c_str());
	if (!adplay->getauthor().empty())
		artist = strdup(adplay->getauthor().c_str());
	if (!adplay->getdesc().empty())
		album = strdup(adplay->getdesc().c_str());
	totaltime = adplay->songlength();
}
