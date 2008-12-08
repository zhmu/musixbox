#include <string>
#include "core/info.h"

#ifndef __LYRICS_H__
#define __LYRICS_H__

//! \brief Maximum length of a single lyrics line
#define LYRICS_MAX_LINE_LENGTH 255

class Lyrics {
public:
	/*! \brief Initialize the Lyrics class
	 *  \param path Path to executable to use for fetching lyrics
	 */
	inline Lyrics(std::string path) { this->path = path; }

	/*! \brief Fetch the lyrics
	 *  \param i Information object to use
	 */
	bool fetch(Info* i);

	//! \brief Retrieve the lyrics
	inline std::string getLyrics() { return lyrics; }

	//! \brief Retrieve the count of lyrices lines
	inline unsigned int getNumLines() { return numlines; }

protected:
	/*! \brief Executable used to fetch the lyrics */
	std::string path;

	/*! \brief The lyrics! */
	std::string lyrics;

	/*! \brief Number of lines in the lyrics */
	unsigned int numlines;
};

#endif /* __LYRICS_H__ */
