#include <vorbis/vorbisfile.h>
#include "decode.h"

#ifndef __DECODE_OGG_H__
#define __DECODE_OGG_H__

/*! \brief Handles decoding of Ogg Vorbis files
 *
 *  The decoder used is libvorbisfile.
 */
class DecoderOgg : public Decoder {
public:
	DecoderOgg(Player* p, Input* i, Output* o, Visualizer* v);
	~DecoderOgg();

	//! \brief Performs a Ogg Vorbis decoding run
	void run();

	//! \brief Retrieve a list of file extensions this class can decode
	static std::list<std::string> getExtensions();

	//! \brief Retrieve Ogg Vorbuis comments
	struct vorbis_comment* getComments();

private:
	struct OggVorbis_File ovf;
};

#endif /* __DECODE_OGG_H__ */
