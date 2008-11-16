#include <sidplay/sidplay2.h>
#include "decode.h"

#ifndef __DECODE_SID_H__
#define __DECODE_SID_H__

#define DECODE_SID_BUFFER_LENGTH	8192

/*! \brief Handles decoding of C64 SID files
 *
 *  The decoder used is libsidplay2
 */
class DecoderSID : public Decoder {
	friend class InfoSID;
public:
	DecoderSID(Player* p, Input* i, Output* o, Visualizer* v);
	~DecoderSID();

	//! \brief Performs a SID decoding run
	void run();

protected:
	const SidTuneInfo& getTuneInfo() { return *player.info().tuneInfo; }

private:
	//! \brief The tune itself
	SidTune tune;

	//! \brief The player
	sidplay2 player;

	//! \brief Decode output buffer
	char* buffer;
};


#endif /* __DECODE_SID_H__ */
