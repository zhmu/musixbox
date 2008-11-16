#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sidplay/sidplay2.h>
#include <sidplay/builders/resid.h>
#include "decode_sid.h"
#include "exceptions.h"

DecoderSID::DecoderSID(Player* p, Input* i, Output* o, Visualizer* v) :
	Decoder(p, i, o, v),
	tune(0)
{
	char* ptr;
	sid2_config_t conf;

	// Check whether the input is seekable - if not, we can't load it
	if (!input->getLength())
		throw DecoderException(std::string("DecoderSID: not a seekable input"));

	/*
	 * libsidplay2 does not support streaming of any kind, so all we can do is read
	 * the input in one go and feed it to the player. Luckily, C64 tunes are quite
	 * limited (where did you think the 64 stands for anyway :-)
	 */
	ptr = (char*)malloc(input->getLength());
	buffer = (char*)malloc(DECODE_SID_BUFFER_LENGTH);
	if (ptr == NULL || buffer == NULL)
		throw DecoderException(std::string("DecoderSID: out of memory"));
	if (!input->read(ptr, input->getLength())) {
		free(ptr);
		throw DecoderException(std::string("DecoderSID: input error"));
	}

	/* Feed the data to tune parser */
	if (!tune.read((const uint_least8_t*)ptr, (const uint_least32_t)input->getLength())) {
		free(ptr);
		throw DecoderException(std::string("DecoderSID: can't read input"));
	}
	free(ptr);

	/* Tell the player to load out tune */
	tune.selectSong(1);
	if (player.load(&tune))
		throw DecoderException(std::string("DecoderSID: can't load input"));

	/* Create a SID emulator */
	ReSIDBuilder* rs = new ReSIDBuilder("ReSID");
	rs->create(player.info().maxsids);
	rs->filter(false);
	rs->sampling(44100);

	/* We want 16bit stereo sound - for the rest, just do what works */
	conf = player.config();
	conf.frequency = 44100;
	conf.precision = 16;
	conf.playback = sid2_stereo;
	conf.sampleFormat = SID2_LITTLE_SIGNED;
	conf.clockSpeed = SID2_CLOCK_CORRECT;
	conf.clockForced = true;
	conf.sidModel = SID2_MODEL_CORRECT;
	conf.optimisation = SID2_DEFAULT_OPTIMISATION;
	conf.sidSamples = true;
	conf.clockDefault = SID2_CLOCK_PAL;
	conf.sidDefault = SID2_MOS6581;
	conf.sidEmulation = rs;
	player.config(conf);
}


DecoderSID::~DecoderSID()
{
	if (buffer != NULL)
		free(buffer);
}

void
DecoderSID::run()
{
	while (!terminating) {
		handlePause();
		unsigned int len = player.play(buffer, DECODE_SID_BUFFER_LENGTH);

		if (visualizer != NULL)
			visualizer->update(buffer, len);

		output->play(buffer, len);

		playingtime = player.time() / player.timebase();
	}
}
