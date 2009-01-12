#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lyrics.h"

using namespace std;

bool
Lyrics::fetch(Info* info)
{
	/* If no path was configured, move along. Nothing to see here... */
	if (path == "")
		return false;

	/* Remove any old lyrics */
	lyrics = ""; numlines = 0;

	/* If the information object is not supplied or empty, nothing to do */
	if (info == NULL || info->getArtist() == NULL || info->getTitle() == NULL)
		return false;

#define ADD(x) \
	s += ' '; s += '\''; \
	if (x != NULL) \
		for (const char* ptr = x; *ptr != '\0'; ptr++) { \
			if (*ptr == '\'') { \
				s += '\''; \
				s += '\\'; \
				s += '\''; \
				s += '\''; \
			} else { \
				s += *ptr; \
			} \
		}  \
	s += '\'';

	/*
	 * Construct the command to use: cmd artist title album
	 * The ADD() macro is used to escape ' to keep the shell
	 * happy.
	 */
	string s = path;
	ADD(info->getArtist());
	ADD(info->getTitle());
	ADD(info->getAlbum());

	/* Redirect stderr, so we'll get error messages too */
	s += " 2>&1";

#undef ADD

	FILE* f = popen(s.c_str(), "r");
	if (f == NULL)
		return false;
	/*
	 * Add the output line-by-line, keeping track fo the line count for the
	 * interface.
	 */
	char line[LYRICS_MAX_LINE_LENGTH + 1];
	while (fgets(line, LYRICS_MAX_LINE_LENGTH, f)) {
		lyrics += line; numlines++;
	}
	fclose(f);
	return true;
}
