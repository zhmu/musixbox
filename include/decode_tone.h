#include "decode.h"

#ifndef __DECODE_TONE_H__
#define __DECODE_TONE_H__

#define PLAYER_BUF_SIZE	8192

class DecoderTone : public Decoder {
public:
	DecoderTone() { freq = 1000; }
	int run();

	void setFrequency(float f) { freq = f; }

private:
	float freq;
};

#endif /* __DECODE_MP3_H__ */
