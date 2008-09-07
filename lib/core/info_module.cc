#include <string.h>
#include <mikmod.h>
#include "decode_module.h"
#include "info_module.h"

void
InfoModule::load(std::string res)
{
	artist = strdup(""); album = strdup("");
	title = Player_LoadTitle((CHAR*)res.c_str());
}
