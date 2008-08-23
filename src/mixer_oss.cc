#include <sys/soundcard.h>
#include <fcntl.h>
#include <unistd.h>
#include "mixer_oss.h"

MixerOSS::MixerOSS(std::string device)
	 : Mixer(device)
{
	fd = open(device.c_str(), O_RDWR);
	if (fd < 0)
		throw NULL; /* XXX */
}

MixerOSS::~MixerOSS()
{
	if (fd >= 0)
		close(fd);
}

unsigned int
MixerOSS::getVolume()
{
	unsigned int i;

	if (ioctl(fd, MIXER_READ(SOUND_MIXER_VOLUME), &i) < 0)
		throw NULL; /* XXX */

	return i >> 8;
}

void
MixerOSS::setVolume(unsigned int volume)
{
	unsigned int i;

	if (volume > 100)
		volume = 100;

	i = volume | (volume << 8);

	if (ioctl(fd, MIXER_WRITE(SOUND_MIXER_VOLUME), &i) < 0)
		throw NULL; /* XXX */
}
