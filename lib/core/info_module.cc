#include <string.h>
#include <mikmod.h>
#include "decode_module.h"
#include "info_module.h"

void
InfoModule::load(const char* fname)
{
	artist = strdup(""); album = strdup("");
	title = Player_LoadTitle((CHAR*)fname);
}
