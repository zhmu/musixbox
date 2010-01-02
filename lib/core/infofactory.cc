#include <algorithm>
#include <string.h>
#include "config.h"
#include "core/decoderfactory.h"
#include "core/infofactory.h"
#include "core/exceptions.h"
#ifdef WITH_MIKMOD
#include "core/decode_module.h"
#include "core/info_module.h"
#endif
#ifdef WITH_ID3TAG
#include "core/info_mp3.h"
#endif
#ifdef WITH_ID3LIB
#include "core/info_mp3_id3lib.h"
#endif

using namespace std;

void
InfoFactory::construct(string resource, Info** info)
{
	*info = NULL;

	/*
	 * Some file formats do not need a decoder in order to obtain the file
	 * information; short-circuit these by only constructing the information
	 * object.
	 */
	string extension = string(resource.begin() + resource.find_last_of(".") + 1, resource.end());
	transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	try {
#ifdef WITH_MIKMOD
		if (DecoderFactory::checkExtension(DecoderModule::getExtensions(), extension)) {
			*info = new InfoModule(NULL);
		} else
#endif /* WITH_MIKMOD */
#if defined(WITH_ID3LIB) || defined(WITH_MPG123) || defined(WITH_ID3TAG)
		{
			/* assume MP3 */
#ifdef WITH_ID3LIB
			*info = new InfoMP3_ID3Lib(NULL);
#elif defined(WITH_ID3TAG)
			*info = new InfoMP3(NULL);
#else /* defined(WITH_MPG123) */
			*info = new InfoMP3_MPG123(NULL);
#endif /* WITH_ID3LIB */
#else
		{
			return;
#endif /* !WITH_MAD && !WITH_MPG123 */
		}
		if (*info != NULL) {
			(*info)->load(resource.c_str());
			return;
		}
	} catch (InfoException &e) {
		/*
		 * Couldn't obtain information - bail out, as it's unlikely that a decoder
		 * factory can...
		 */
		*info = NULL;
		return;
	}

	/*
	 * As a last resort, just ask the decoder factory for the information and
	 * throw everything else away.
	 */
	Input* input;
	Decoder* decoder;
	DecoderFactory::construct(resource, NULL, NULL, NULL, &input, &decoder, info);
	if (input != NULL) delete input;
	if (decoder != NULL) delete decoder;
}

/* vim:set ts=2 sw=2: */
