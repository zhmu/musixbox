#include <string>
#include "decode.h"

#ifndef __INFO_H__
#define __INFO_H__

/*! \brief Abstract information object
 *
 *  An information object is responsible for acquiring track information,
 *  such as the title, length and bitrate.
 */
class Info {
public:
	Info(Decoder* d);
	virtual ~Info();

	/*! \brief Load information from a resource
	 *  \param res Resource to process
	 */
	virtual void load(std::string res ) = 0;

	//! \brief Retrieve number of seconds this file lasts
	inline virtual int getTotalTime() { return totaltime; }

	//! \brief Retrieve artist name
	inline virtual const char* getArtist() { return artist; }

	//! \brief Retrieve album
	inline virtual const char* getAlbum() { return album; }

	//! \brief Retrieve song title
	inline virtual const char* getTitle() { return title; }

	//! \brief Retrieve track year
	inline virtual int getYear() { return year; }

	//! \brief Retrieve track number
	inline virtual int getNr() { return nr; }
	
protected:
	//! \brief Decoder object we belong to
	Decoder* decoder;

	//! \brief Number of seconds this song lasts
	int totaltime;

	//! \brief Name of the artist
	char* artist;

	//! \brief Name of the album
	char* album;

	//! \brief Title of the song
	char* title;

	//! \brief Year of the song
	int year;

	//! \brief Track number of the song
	int nr;
};

#endif /* __INFO_H__ */
