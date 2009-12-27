#include "info.h"
#include "decode_flac.h"

#ifndef __INFO_MP3_MPG123_H__
#define __INFO_MP3_MPG123_H__

//! \brief Retrieve information on MP3 files using libmpg123
class InfoMP3_MPG123 : public Info {
public:
	InfoMP3_MPG123 (Decoder* d) : Info(d) { }

	/*! \brief Load information from a resource
	 *  \param res Resource to process
	 */
	inline void load(std::string fname) { }

	//! \brief Retrieve artist name
	inline const char* getArtist() {
		return (reinterpret_cast<DecoderMP3_MPG123*> (decoder))->getArtist();
	}

	//! \brief Retrieve album
	inline const char* getAlbum() {
		return (reinterpret_cast<DecoderMP3_MPG123*> (decoder))->getAlbum();
	}

	//! \brief Retrieve song title
	inline const char* getTitle() {
		return (reinterpret_cast<DecoderMP3_MPG123*> (decoder))->getTitle();
	}

	//! \brief Retrieve song year
	inline int getYear() {
		return (reinterpret_cast<DecoderMP3_MPG123*> (decoder))->getYear();
	}

	//! \brief Retrieve track number
	inline int getNr() {
		return (reinterpret_cast<DecoderMP3_MPG123*> (decoder))->getNr();
	}
};

#endif /* __INFO_MP3_MPG123_H__ */
