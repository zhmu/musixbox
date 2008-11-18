#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include "exceptions.h"
#include "output_oss.h"

using namespace std;

OutputOSS::OutputOSS() : Output()
{
	int tmp;

	// Simply open the device
	fd = open("/dev/dsp0", O_RDWR);
	if (fd < 0)
		throw OutputException("can't open audio device");

	/* We want 16 bits, 2 channels (stereo) at 44.1kHz output */
	tmp = 16;
	if (ioctl(fd, SNDCTL_DSP_SETFMT, &tmp) < 0) {
		close(fd);
		throw OutputException("can't set 16 bits output");
	}
	tmp = 2;
	if (ioctl(fd, SNDCTL_DSP_CHANNELS, &tmp) < 0) {
		close(fd);
		throw OutputException("can't set stereo output");
	}
	tmp = 44100;
	if (ioctl(fd, SNDCTL_DSP_SPEED, &tmp) < 0) {
		close(fd);
		throw OutputException("can't set 44.1kHz output");
	}
}

void
OutputOSS::play(char* buf, size_t len)
{
	ssize_t s;

	/*
	 * Write the whole buffer - a partial write may occur if the OS
	 * cannot buffer anymore, so be nice to it and feed it more data
	 * later on.
	 */
	while (len > 0) {
		s = write(fd, buf, len);
		if (s <= 0)
			// Something went wrong - best to bail out
			break;
		len -= s;
		buf += s;
	}

}

OutputOSS::~OutputOSS()
{
	close(fd);
}
