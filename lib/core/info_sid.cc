#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "decode_sid.h"
#include "info_sid.h"

void
InfoSID::load(const char* fname)
{
	const SidTuneInfo tuneinfo = (reinterpret_cast<DecoderSID*> (decoder))->getTuneInfo();

	if (tuneinfo.numberOfInfoStrings == 0)
		return;

	title  = strdup(tuneinfo.infoString[0]);
	artist = strdup(tuneinfo.infoString[1]);
	album  = strdup(tuneinfo.infoString[2]);
}
