#ifndef __INFO_H__
#define __INFO_H__

/*! \brief Abstract information object
 *
 *  An information object is responsible for acquiring track information,
 *  such as the title, length and bitrate.
 */
class Info {
public:
	Info();
	~Info();

	//! \brief Process an input file
	virtual int load(const char* fname) = 0;

	//! \brief Retrieve number of seconds this file lasts
	inline int getTotalTime() { return totaltime; }

	//! \brief Retrieve artist name
	inline const char* getArtist() { return artist; }

	//! \brief Retrieve album
	inline const char* getAlbum() { return album; }

	//! \brief Retrieve song title
	inline const char* getTitle() { return title; }
	

protected:
	//! \brief Number of seconds this song lasts
	int totaltime;

	//! \brief Name of the artist
	char* artist;

	//! \brief Name of the album
	char* album;

	//! \brief Title of the song
	char* title;
};

#endif /* __INFO_H__ */
