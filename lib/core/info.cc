#include <stdlib.h>
#include "info.h"

using namespace std;

Info::Info(Decoder* d) {
	totaltime = 0; decoder = d;
	artist = NULL; album = NULL; title = NULL;
}

Info::~Info() {
	if (artist != NULL) free(artist);
	if (album != NULL) free(album);
	if (title != NULL) free(title);
}
