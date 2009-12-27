#include <mpg123.h>
#include "decode.h"

#ifndef __DECODE_MP3_MPG123_H__
#define __DECODE_MP3_MPG123_H__

/*! \brief Handles decoding of MP3 files
 *
 *  The decoder used is libmpg123
 */
class DecoderMP3_MPG123 : public Decoder {
	friend class InfoMP3_MPG123;
public:
	DecoderMP3_MPG123(Player* p, Input* i, Output* o, Visualizer* v);
	~DecoderMP3_MPG123();

	//! \brief Performs a mp3 decoding run
	void run();

	//! \brief Retrieve a list of file extensions this class can decode
	static std::list<std::string> getExtensions();

protected:
	const char* getArtist() { return artist; }
	const char* getAlbum()  { return album; }
	const char* getTitle()  { return title; }
	int getYear()           { return year; }
	int getNr()             { return tracknr; }

private:
	mpg123_handle* handle;
	mpg123_id3v1* v1;
	mpg123_id3v2* v2;
	bool handled_id3;

	char* artist;
	char* album;
	char* title;
	int year;
	int tracknr;
};

#endif /* __DECODE_MP3_MPG123_H__ */
