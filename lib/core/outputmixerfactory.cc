#include "config.h"
#include "core/exceptions.h"
#include "core/output.h"
#ifdef WITH_AO
#include "core/output_ao.h"
#endif
#ifdef WITH_ALSA
#include "core/output_alsa.h"
#endif
#include "core/output_swmixer.h"
#include "core/output_null.h"
#include "core/output_oss.h"
#include "core/outputmixerfactory.h"
#include "core/mixer.h"
#include "core/mixer_oss.h"
#include "core/mixer_sw.h"

using namespace std;

void
OutputMixerFactory::construct(string resource, Output** output, Mixer** mixer)
{
	*output = NULL; *mixer = NULL;
	if (resource == "null") {
		*output = new OutputNull();
	} else if (resource == "oss") {
		*output = new OutputOSS();
		*mixer = new MixerOSS("/dev/mixer0" /* XXX */);
#ifdef WITH_AO
	} else if (resource == "ao") {
		*output = new OutputAO();
		try {
			*mixer = new MixerOSS("/dev/mixer0" /* XXX */);
		} catch (MixerException& e) {
			/* Lack of a mixer shouldn't be fatal */
			*mixer = NULL;
		}
	} else if (resource == "ao-swmixer") {
		*output = new OutputSWMixer(new OutputAO());
		*mixer = new MixerSW((OutputSWMixer*)*output);
#endif /* WITH_AO */
#ifdef WITH_ALSA
	} else if (resource == "alsa") {
		*output = new OutputALSA();
		*mixer = NULL;
#endif /* WITH_ALSA */
	} else 
		throw FactoryException("unsupport output requested");
}

void
OutputMixerFactory::getAvailable(list<string>& o)
{
	o.push_front("null");
#ifdef WITH_AO
	o.push_front("ao");
	o.push_front("ao-swmixer");
#endif
#ifdef WITH_ALSA
	o.push_front("alsa");
#endif
	o.push_front("oss");
}
