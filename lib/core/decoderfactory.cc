#include <string.h>
#include "config.h"
#include "decoderfactory.h"
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
#include "core/input_file.h"
#ifdef WITH_CURL
#include "core/input_remote.h"
#endif
#include "core/exceptions.h"

void
DecoderFactory::construct(std::string resource, Output* output, Visualizer* visualizer, Input** input, Decoder** decoder, Info** info)
{
	/* Initially, nothing has been constructed yet */
	*input = NULL; *decoder = NULL; *info = NULL;

#ifdef WITH_CURL
	/*
	 * If we find :// in the filename and CURL is available, assume we
	 * are playing a stream.
	 */
	if (resource.find("://") != std::string::npos) {
		*input = new InputRemote(resource);
	} else
#endif
		*input = new InputFile(resource);

	std::string extension = std::string(resource.begin() + resource.find_last_of(".") + 1, resource.end());
	try {
#ifdef WITH_VORBIS
		if (!strcasecmp(extension.c_str(), "ogg")) {
			*decoder = new DecoderOgg(*input, output, visualizer);
			*info = new InfoOgg(*decoder);
		} else
#endif /* WITH_VORBIS */
#ifdef WITH_FLAC
		if (!strcasecmp(extension.c_str(), "flac")) {
			*decoder = new DecoderFLAC(*input, output, visualizer);
			*info = new InfoFLAC(*decoder);
		} else
#endif /* WITH_FLAC */
#ifdef WITH_MIKMOD
		if (!strcasecmp(extension.c_str(), "669")  || !strcasecmp(extension.c_str(), "amf") ||
		    !strcasecmp(extension.c_str(), "apun") || !strcasecmp(extension.c_str(), "dsm") ||
		    !strcasecmp(extension.c_str(), "far")  || !strcasecmp(extension.c_str(), "gdm") ||
		    !strcasecmp(extension.c_str(), "it")   || !strcasecmp(extension.c_str(), "imf") ||
		    !strcasecmp(extension.c_str(), "mod")  || !strcasecmp(extension.c_str(), "med") ||
		    !strcasecmp(extension.c_str(), "mtm")  || !strcasecmp(extension.c_str(), "okt") ||
		    !strcasecmp(extension.c_str(), "s3m")  || !strcasecmp(extension.c_str(), "stm") ||
		    !strcasecmp(extension.c_str(), "stx")  || !strcasecmp(extension.c_str(), "ult") ||
		    !strcasecmp(extension.c_str(), "uni")  || !strcasecmp(extension.c_str(), "xm")) {
			*decoder = new DecoderModule(*input, output, visualizer);
			*info = new InfoModule(*decoder);
		} else
#endif /* WITH_MIKMOD */
#ifdef WITH_SIDPLAY2
		if (!strcasecmp(extension.c_str(), "sid")) {
			*decoder = new DecoderSID(*input, output, visualizer);
			*info = new InfoSID(*decoder);
		} else
#endif /* WITH_SIDPLAY2 */
#ifdef WITH_MAD
		{
			/* assume MP3 */
			*decoder = new DecoderMP3(*input, output, visualizer);
#ifdef WITH_ID3TAG
			*info = new InfoMP3(*decoder);
#endif
#else /* WITH_MAD */
		{
			delete *input; *input = NULL;
			return;
#endif /* !WITH_MAD */
		}
		if (*info != NULL)
			(*info)->load(resource.c_str());
	} catch (InfoException &e) {
		/* Failure to obtain information should not be critical */
		*info = NULL;
	}
}
