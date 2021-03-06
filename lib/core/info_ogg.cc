#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "decode_ogg.h"
#include "info_ogg.h"

using namespace std;

void
InfoOgg::load(string res)
{
	struct vorbis_comment* c = (reinterpret_cast<DecoderOgg*> (decoder))->getComments();

	for (int i = 0; i < c->comments; i++) {
		char* ptr = strchr(c->user_comments[i], '=');
		if (ptr == NULL)
			continue;

		if (!strncasecmp(c->user_comments[i], "TITLE", ptr - c->user_comments[i])) {
			title = strdup(ptr + 1);
		}
		if (!strncasecmp(c->user_comments[i], "ARTIST", ptr - c->user_comments[i])) {
			artist = strdup(ptr + 1);
		}
		if (!strncasecmp(c->user_comments[i], "ALBUM", ptr - c->user_comments[i])) {
			album = strdup(ptr + 1);
		}
		if (!strncasecmp(c->user_comments[i], "TRACKNUMBER", ptr - c->user_comments[i])) {
			nr = atoi(ptr + 1);
		}
		if (!strncasecmp(c->user_comments[i], "YEAR", ptr - c->user_comments[i])) {
			year = atoi(ptr + 1);
		}
	}
}
