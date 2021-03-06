#include "decode.h"

#ifndef __DECODE_TONE_H__
#define __DECODE_TONE_H__

#define PLAYER_BUF_SIZE	8192

//! \brief Simple tone generator 
class DecoderTone : public Decoder {
public:
	DecoderTone(Player* p, Input* i, Output* o, Visualizer* v) : Decoder(p, i, o, v) { freq = 1000; }

	//! \brief Performs a tone generator run
	void run();

	//! \brief Set the frequency generated
	void setFrequency(float f) { freq = f; }

private:
	//! \brief Frequency (in Hz) to generate
	float freq;
};

#endif /* __DECODE_MP3_H__ */
