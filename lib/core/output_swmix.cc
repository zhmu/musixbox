#include "exceptions.h"
#include "output_swmixer.h"

using namespace std;

OutputSWMixer::OutputSWMixer(Output* o) : Output()
{
	output = o; volume = 100;
}

OutputSWMixer::~OutputSWMixer()
{
	delete output;
}

void
OutputSWMixer::play(char* buf, size_t len)
{
	/*
	 * Software mixing is actually quite easy: if we assume native values
	 * are at maximum volume, we just need to linearly scale the values
	 * down. As we define 100 to be the maximum, multiplying them by
	 * volume / 100 will work nicely.
	 *
	 * Note: int16_t because input is expected to signed 16 bit PCM values.
	 */
	int16_t* ptr = (int16_t*)buf;
	for (size_t i = 0; i < len / 2; i++, ptr++)
		*ptr = (int16_t)((float)*ptr * ((float)volume / 100.0f));
	output->play(buf, len);
}

void
OutputSWMixer::setVolume(unsigned int vol)
{
	/* wrap to 0 - 100 */
	volume = (vol > 100) ? 100 : vol;
}
