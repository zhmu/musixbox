#include <stdlib.h>
#include "info.h"

Info::Info() {
	totaltime = 0;
	artist = NULL; album = NULL; title = NULL;
}

Info::~Info() {
	if (artist != NULL) free(artist);
	if (album != NULL) free(album);
	if (title != NULL) free(title);
}
