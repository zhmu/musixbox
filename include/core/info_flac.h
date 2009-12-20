#include "info.h"
#include "decode_flac.h"

#ifndef __INFO_FLAC_H__
#define __INFO_FLAC_H__

//! \brief Retrieve information on FLAC files
class InfoFLAC : public Info {
public:
	InfoFLAC(Decoder* d) : Info(d) { }

	/*! \brief Load information from a resource
	 *  \param res Resource to process
	 */
	void load(std::string fname);

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

	//! \brief Retrieve song year
	inline int getYear() {
		return (reinterpret_cast<DecoderFLAC*> (decoder))->getYear();
	}

	//! \brief Retrieve track number
	inline int getNr() {
		return (reinterpret_cast<DecoderFLAC*> (decoder))->getNr();
	}
};

#endif /* __INFO_FLAC_H__ */
