#include <stdlib.h>
#include <stdio.h>
#include <id3tag.h>
#include "exceptions.h"
#include "info_mp3.h"

using namespace std;

void
InfoMP3::load(string res)
{
	struct id3_file* id3;
	struct id3_tag* tag;
	struct id3_frame* frame;

	id3 = id3_file_open(res.c_str(), ID3_FILE_MODE_READONLY);
	if (id3 == NULL)
		throw InfoException(string("InfoMP3: Unable to open ") + res);
	tag = id3_file_tag(id3);

	/* Handle all tags, one by one... */
	frame = id3_tag_findframe(tag, ID3_FRAME_TITLE, 0);
	if (frame != NULL) {
		title = (char*)id3_ucs4_latin1duplicate(id3_field_getstrings(&frame->fields[1], 0));
	}
	frame = id3_tag_findframe(tag, ID3_FRAME_ARTIST, 0);
	if (frame != NULL) {
		artist = (char*)id3_ucs4_latin1duplicate(id3_field_getstrings(&frame->fields[1], 0));
	}
	frame = id3_tag_findframe(tag, ID3_FRAME_ALBUM, 0);
	if (frame != NULL) {
		album = (char*)id3_ucs4_latin1duplicate(id3_field_getstrings(&frame->fields[1], 0));
	}
	
	id3_file_close(id3);
}
