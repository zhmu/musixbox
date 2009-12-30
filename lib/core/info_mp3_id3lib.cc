#include <stdlib.h>
#include <stdio.h>
#include <id3/tag.h>
#include <id3/misc_support.h>
#include "exceptions.h"
#include "info_mp3_id3lib.h"

using namespace std;

void
InfoMP3_ID3Lib::load(string res)
{
	ID3_Tag* tag = new ID3_Tag(res.c_str());

	title = safe_strdup(ID3_GetTitle(tag));
	album = safe_strdup(ID3_GetAlbum(tag));
	artist = safe_strdup(ID3_GetArtist(tag));

	char* strYear = ID3_GetYear(tag);
	if (strYear != NULL)
		year = atoi(strYear);
	char* strTrackNr = ID3_GetTrack(tag);
	if (strTrackNr != NULL)
		nr = atoi(strTrackNr);

	delete tag;
}

char*
InfoMP3_ID3Lib::safe_strdup(const char* s)
{
	return s != NULL ? strdup(s) : NULL;
}

/* vim:set ts=2 sw=2: */
