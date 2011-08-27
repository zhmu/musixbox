#include <stdio.h>
#include <stdlib.h>
#include "decode_adlib.h"
#include "exceptions.h"

using namespace std;

#define RATE 44100
#define BUFSIZE 512 /* buffer size in samples */

/*
 * The AdPlug library heavily depends on being able to seek into a
 * stream; it also has its own binary I/O stream library to avoid
 * any platform-specific ssues. We just load the entire input to a
 * buffer and streaming from that buffer instead - the tunes aren't
 * that large so this should be fine.
 */
DecoderAdLib::FileProviderHelper::FileProviderHelper(Input* input)
{
	unsigned long stream_length = input->getLength();
	if (!stream_length)
		throw DecoderException(string("DecoderAdLib: not a seekable input"));

	stream_buffer = malloc(stream_length);
	if (stream_buffer == NULL)
		throw DecoderException(string("DecoderAdLib: out of memory"));
	if (!input->read((char*)stream_buffer, stream_length)) {
		free(stream_buffer);
		throw DecoderException(string("DecoderAdLib: input error"));
	}

	stream = new binisstream(stream_buffer, stream_length);
}

DecoderAdLib::FileProviderHelper::~FileProviderHelper()
{
	delete stream;
	free(stream_buffer);
}

DecoderAdLib::DecoderAdLib(Player* p, Input* i, Output* o, Visualizer* v) :
	Decoder(p, i, o, v),
	opl(RATE, true, false) /* 44.1kHz 16 bit mono */
	
{
	fileProvider = new FileProviderHelper(i);

	/* Initialize AdPlug */
	adplayer = CAdPlug::factory(i->getResource(), &opl, CAdPlug::players, *fileProvider);
	if (adplayer == NULL) {
		delete fileProvider;
		throw DecoderException(string("DecoderAdLib: cannot play this file"));
	}

	/* Construct the OPL/play buffers */
	play_buffer = new char[BUFSIZE * 4 /* 16-bit stereo */];
	opl_buffer = new short[BUFSIZE /* 16-bit mono */];
}

DecoderAdLib::~DecoderAdLib()
{
	delete[] opl_buffer;
	delete[] play_buffer;
	delete fileProvider;
}

void
DecoderAdLib::run()
{
	while (!terminating) {
		handlePause();
		if (!adplayer->update())
			break;

		int chunk_len;
		for (int left = RATE / adplayer->getrefresh(); left > 0; left -= chunk_len) {
			chunk_len = std::min(left, BUFSIZE);
			opl.update(opl_buffer, chunk_len);

			/* OPL buffer is in 16-bit mono samples - we need to convert it to stereo */
			for (int i = 0; i < chunk_len; i++) {
				play_buffer[i * 4 + 0] = opl_buffer[i] & 0xff;
				play_buffer[i * 4 + 1] = opl_buffer[i] >> 8;
				play_buffer[i * 4 + 2] = opl_buffer[i] & 0xff;
				play_buffer[i * 4 + 3] = opl_buffer[i] >> 8;
			}
			output->play(play_buffer, chunk_len * 4 /* 16-bit stereo */);
		}
	}
}

list<string> 
DecoderAdLib::getExtensions()
{
	list<string> l;
	l.push_back("a2m"); l.push_back("adl"); l.push_back("amd");
	l.push_back("ram"); l.push_back("cff"); l.push_back("cmf");
	l.push_back("d00"); l.push_back("dfm"); l.push_back("dmo");
	l.push_back("dro"); l.push_back("dtm"); l.push_back("hsc");
	l.push_back("hsp"); l.push_back("imf"); l.push_back("ksm");
	l.push_back("laa"); l.push_back("lds"); l.push_back("m");
	l.push_back("mad"); l.push_back("mid"); l.push_back("mkj");
	l.push_back("msc"); l.push_back("mtk"); l.push_back("rad");
	l.push_back("raw"); l.push_back("rix"); l.push_back("rol");
	l.push_back("s3m"); l.push_back("sa2"); l.push_back("sat");
	l.push_back("sci"); l.push_back("sng"); l.push_back("sxad");
	l.push_back("xms"); l.push_back("xsm");
	return l;
}
