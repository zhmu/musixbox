#include "FLAC++/decoder.h"
#include "decode.h"

#ifndef __DECODE_FLAC_H__
#define __DECODE_FLAC_H__

/*! \brief Handles decoding of FLAC files
 *
 *  The decoder used is libFLAC
 */
class DecoderFLAC : public Decoder, public FLAC::Decoder::Stream {
public:
	DecoderFLAC(Input* i, Output* o, Visualizer* v);
	~DecoderFLAC();

	//! \brief Performs a FLAC decoding run
	int run();

protected:
	//! \brief Read callback
	FLAC__StreamDecoderReadStatus read_callback(FLAC__byte buffer[], size_t* bytes);

	//! \brief Write callback
	FLAC__StreamDecoderWriteStatus write_callback(const FLAC__Frame* frame, const FLAC__int32 *const buffer[]);

	//! \brief Metadata callback
	void metadata_callback(const FLAC__StreamMetadata* metadata);

	//! \brief Error callback
	void error_callback(FLAC__StreamDecoderErrorStatus status);
};

#endif /* __DECODE_FLAC_H__ */
