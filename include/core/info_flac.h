#include "info.h"
#include "decode_flac.h"

#ifndef __INFO_FLAC_H__
#define __INFO_FLAC_H__

//! \brief Retrieve information on FLAC files
class InfoFLAC : public Info {
public:
	InfoFLAC(Decoder* d) : Info(d) { }

	//! \brief Process an input file
	void load(const char* fname);

	//! \brief Retrieve artist name
	inline const char* getArtist() {
		return (reinterpret_cast<DecoderFLAC*> (decoder))->getArtist();
	}

	//! \brief Retrieve album
	inline const char* getAlbum() {
		return (reinterpret_cast<DecoderFLAC*> (decoder))->getAlbum();
	}

	//! \brief Retrieve song title
	inline const char* getTitle() {
		return (reinterpret_cast<DecoderFLAC*> (decoder))->getTitle();
	}
};

#endif /* __INFO_FLAC_H__ */
