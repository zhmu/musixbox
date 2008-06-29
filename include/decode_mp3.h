#include <mad.h>
#include "decode.h"

#ifndef __DECODE_MP3_H__
#define __DECODE_MP3_H__

#define CHUNK_SIZE	4096

class DecoderMP3 : public Decoder {
public:
	DecoderMP3();
	~DecoderMP3();

	int run();

private:
	int handleInput(struct mad_stream* stream);
	int convertOutput(char* buf, int* len, struct mad_header const* header, struct mad_pcm* pcm);
	signed int scaleFrequency(mad_fixed_t s);

	FILE* musicfile;
	char* music_chunk;

};

#endif /* __DECODE_MP3_H__ */
