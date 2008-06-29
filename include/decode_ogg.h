#include "decode.h"

#ifndef __DECODE_OGG_H__
#define __DECODE_OGG_H__

/*! \brief Handles decoding of Ogg Vorbis files
 *
 *  The decoder used is libvorbisfile.
 */
class DecoderOgg : public Decoder {
public:
	//! \brief Performs a Ogg Vorbis decoding run
	int run();
};

#endif /* __DECODE_OGG_H__ */
