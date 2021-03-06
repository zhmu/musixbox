#include <algorithm>
#include <string.h>
#include "config.h"
#include "decoderfactory.h"
#include "core/decode_tone.h"
#ifdef WITH_MAD
#include "core/decode_mp3.h"
#endif
#ifdef WITH_ID3TAG
#include "core/info_mp3.h"
#endif
#ifdef WITH_VORBIS
#include "core/decode_ogg.h"
#include "core/info_ogg.h"
#endif
#ifdef WITH_FLAC
#include "core/decode_flac.h"
#include "core/info_flac.h"
#endif
#ifdef WITH_MIKMOD
#include "core/decode_module.h"
#include "core/info_module.h"
#endif
#ifdef WITH_SIDPLAY2
#include "core/decode_sid.h"
#include "core/info_sid.h"
#endif
#ifdef WITH_ADPLUG
#include "core/decode_adlib.h"
#include "core/info_adlib.h"
#endif
#ifdef WITH_SDL_MIXER
#include "core/decode_midi.h"
#endif
#include "core/input_file.h"
#ifdef WITH_CURL
#include "core/input_remote.h"
#endif
#ifdef WITH_MPG123
#include "core/decode_mp3_mpg123.h"
#include "core/info_mp3_mpg123.h"
#endif
#ifdef WITH_ID3LIB
#include "core/info_mp3_id3lib.h"
#endif
#include "core/exceptions.h"

using namespace std;

void
DecoderFactory::construct(string resource, Player* player, Output* output, Visualizer* visualizer, Input** input, Decoder** decoder, Info** info)
{
	/* Initially, nothing has been constructed yet */
	*input = NULL; *decoder = NULL; *info = NULL;

	/*
	 * If the resource starts with 'tone:', assume the tone generator is to be
	 * used.
	 */
	if (resource.find("tone:") == 0) {
		float tone = atof(resource.c_str() + 5);
		if (tone > 0.0f) {
			*decoder = new DecoderTone(player, NULL, output, visualizer);
			(dynamic_cast<DecoderTone*>(*decoder))->setFrequency(tone);
			return;
		}
	}

#ifdef WITH_CURL
	/*
	 * If we find :// in the filename and CURL is available, assume we
	 * are playing a stream.
	 */
	if (resource.find("://") != string::npos) {
		*input = new InputRemote(resource);
	} else
#endif
		*input = new InputFile(resource);

	string extension = string(resource.begin() + resource.find_last_of(".") + 1, resource.end());
	transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	try {
#ifdef WITH_VORBIS
		if (checkExtension(DecoderOgg::getExtensions(), extension)) {
			*decoder = new DecoderOgg(player, *input, output, visualizer);
			*info = new InfoOgg(*decoder);
		} else
#endif /* WITH_VORBIS */
#ifdef WITH_FLAC
		if (checkExtension(DecoderFLAC::getExtensions(), extension)) {
			*decoder = new DecoderFLAC(player, *input, output, visualizer);
			*info = new InfoFLAC(*decoder);
		} else
#endif /* WITH_FLAC */
#ifdef WITH_SDL_MIXER
		if (checkExtension(DecoderMIDI::getExtensions(), extension)) {
			*decoder = new DecoderMIDI(player, *input, output, visualizer);
		} else
#endif /* WITH_SDL_MIXER */
#ifdef WITH_ADPLUG
		if (checkExtension(DecoderAdLib::getExtensions(), extension)) {
			*decoder = new DecoderAdLib(player, *input, output, visualizer);
			*info = new InfoAdLib(*decoder);
		} else
#endif /* WITH_ADPLUG */
#ifdef WITH_MIKMOD
		if (checkExtension(DecoderModule::getExtensions(), extension)) {
			*decoder = new DecoderModule(player, *input, output, visualizer);
			*info = new InfoModule(*decoder);
		} else
#endif /* WITH_MIKMOD */
#ifdef WITH_SIDPLAY2
		if (checkExtension(DecoderSID::getExtensions(), extension)) {
			*decoder = new DecoderSID(player, *input, output, visualizer);
			*info = new InfoSID(*decoder);
		} else
#endif /* WITH_SIDPLAY2 */
#if defined(WITH_MAD) || defined(WITH_MPG123)
		{
			/* assume MP3 */
#ifdef WITH_MPG123
			*decoder = new DecoderMP3_MPG123(player, *input, output, visualizer);
#ifdef WITH_ID3LIB
			*info = new InfoMP3_ID3Lib(*decoder);
#else
			*info = new InfoMP3_MPG123(*decoder);
#endif /* WITH_ID3LIB */
#else
			*decoder = new DecoderMP3(player, *input, output, visualizer);
#ifdef WITH_ID3LIB
			*info = new InfoMP3_ID3Lib(*decoder);
#elif defined(WITH_ID3TAG)
			*info = new InfoMP3(*decoder);
#endif
#endif
#else /* WITH_MAD && WITH_MPG123 */
		{
			delete *input; *input = NULL;
			return;
#endif /* !WITH_MAD && !WITH_MPG123 */
		}
		if (*info != NULL)
			(*info)->load(resource.c_str());
	} catch (InfoException &e) {
		/* Failure to obtain information should not be critical */
		*info = NULL;
	}
}

bool
DecoderFactory::checkExtension(std::list<string> extensions, std::string ext)
{
	return find(extensions.begin(), extensions.end(), ext) != extensions.end();
}

void
DecoderFactory::getExtensions(std::list<std::string>& extensions)
{
#define ADD_EXTENSIONS(exts,cl) do {                                      \
	list<string> l = cl::getExtensions();                                   \
	for (list<string>::iterator it = l.begin(); it != l.end(); it++)        \
		exts.push_back(*it);                                                  \
} while (0);

#ifdef WITH_VORBIS
	ADD_EXTENSIONS(extensions, DecoderOgg);
#endif
#ifdef WITH_FLAC
	ADD_EXTENSIONS(extensions, DecoderFLAC);
#endif
#ifdef WITH_MIKMOD
	ADD_EXTENSIONS(extensions, DecoderModule);
#endif
#ifdef WITH_SIDPLAY2
	ADD_EXTENSIONS(extensions, DecoderSID);
#endif
#ifdef WITH_ADPLUG
	ADD_EXTENSIONS(extensions, DecoderAdLib);
#endif
#ifdef WITH_SDL_MIXER
	ADD_EXTENSIONS(extensions, DecoderMIDI);
#endif
#ifdef WITH_MPG123
	ADD_EXTENSIONS(extensions, DecoderMP3_MPG123);
#else
#ifdef WITH_MAD
	ADD_EXTENSIONS(extensions, DecoderMP3);
#endif
#endif
}

/* vim:set ts=2 sw=2: */
