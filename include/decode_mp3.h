#include "decode.h"

#ifndef __DECODE_MP3_H__
#define __DECODE_MP3_H__

#define CHUNK_SIZE	4096

class DecoderMP3 : public Decoder {
public:
	DecoderMP3();
	~DecoderMP3();

	int open(const char*);
	int close();
	int run();

private:
	int handleInput(struct mad_stream* stream);

	FILE* musicfile;
	char* music_chunk;

};

#endif /* __DECODE_MP3_H__ */
