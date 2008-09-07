#include <mad.h>
#include "decode.h"

#ifndef __DECODE_MP3_H__
#define __DECODE_MP3_H__

#define CHUNK_SIZE	4096

#define XING_MAGIC		(uint32_t)(('X' << 24 | 'i' << 16 | 'n' << 8 | 'g'))
#define XING_FLAG_FRAMES	0x0001
#define XING_FLAG_BYTES		0x0002
#define XING_FLAG_TOC		0x0004
#define XING_FLAG_SCALE		0x0008

/*! \brief Handles decoding of MP3 files
 *
 *  The decoder used is libmad.  
 */
class DecoderMP3 : public Decoder {
public:
	DecoderMP3(Input* i, Output* o, Visualizer* v);
	~DecoderMP3();

	//! \brief Performs a mp3 decoding run
	void run();

private:
	/*! \brief Reads an input chunk and feeds it to the stream
	 *
	 *  This function ensures data read by libmad that is not processed
	 *  will not be overwritten.
	 */
	int handleInput(struct mad_stream* stream);

	/*! \brief Converts 24 bit PCM data returned by libmad to 16 bit PCM
	 *  \param buf Output buffer
	 *  \param len Buffer size, must be set before calling this function
	 *  \param header libmad header structure
	 *  \param pcm libmad pcm data
	 *
	 *  Once the function is finished, len is updated to contain the number
	 *  of bytes filled in buf.
	 */
	int convertOutput(char* buf, unsigned int* len, struct mad_header const* header, struct mad_pcm* pcm);

	/*! \brief Scales a 24 bit PCM value to a 16 bit PCM value
	 *  \param s Value to scale
	 *  \return The new scaled value
	 *  \todo The scaling algorithm is very poor and should be fixed  
	 */
	signed int scaleFrequency(mad_fixed_t s);

	//! \brief Attempts to decode a Xing tag
	void parseXingTag(struct mad_stream* stream, struct mad_frame* frame);

	//! \brief Temporary chunk used to ensure unprocessed data is not overwritten
	char* music_chunk;

};

#endif /* __DECODE_MP3_H__ */
