#include <alsa/asoundlib.h>
#include "exceptions.h"
#include "output_alsa.h"

using namespace std;

OutputALSA::OutputALSA() : Output()
{
	int err;
	snd_pcm_hw_params_t* hw_params = NULL;

	/*
	 * This whole structure is just a mess: we obtain a hw_params
	 * structure after roughly two calls, which we fill with information,
	 * subsequently pass on to ALSA and finally we just get rid of it.
	 *
	 * However, any call may fail, so we have to free the hw_params structure
	 * later on. In order to do this semi-cleanly, we throw an exception
	 * which we catch and update later on...
	 */
	try {
		// Attempt to open a PCM device
		err = snd_pcm_open(&sound_device, "default", SND_PCM_STREAM_PLAYBACK, 0);
		if (err < 0)
			throw OutputException("snd_pcm_open(): can't open device");

		// Allocate hardware parameters and fill them
		err = snd_pcm_hw_params_malloc(&hw_params);
		if (err < 0)
			throw OutputException("snd_pcm_hw_params_malloc(): failure");
		err = snd_pcm_hw_params_any(sound_device, hw_params);
		if (err < 0)
			throw OutputException("snd_pcm_hw_params_any(): failure");

		// We want to play interleaved data (L-channel data, R-channel data, etc)
		err = snd_pcm_hw_params_set_access(sound_device, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
		if (err < 0)
			throw OutputException("snd_hw_params_set_rate_resample(): failure");

		/* 16 bit low endian is what we grok */
		err = snd_pcm_hw_params_set_format(sound_device, hw_params, SND_PCM_FORMAT_S16_LE);
		if (err < 0)
			throw OutputException("snd_pcm_hw_params_set_format(): failure");

		/* Stereo will do */
		err = snd_pcm_hw_params_set_channels(sound_device, hw_params, 2);
		if (err < 0)
			throw OutputException("snd_pcm_hw_params_set_channels(): failure");

		/* Play 44.1kHz audio */
		unsigned int rate = 44100;
		err = snd_pcm_hw_params_set_rate_near(sound_device, hw_params, &rate, NULL);
		if (err < 0)
			throw OutputException("snd_pcm_hw_params_set_channels(): failure");

		/*
		 * Request buffering of around 0.5 seconds; otherwise we'll eat
		 * up a lot of CPU time for nothing (and this may cause skips and
		 * cracks on slower hardware...)
		 */
		unsigned int buffer_time = 500000;
		err = snd_pcm_hw_params_set_buffer_time_near(sound_device, hw_params, &buffer_time, 0);
		if (err < 0)
			throw OutputException("snd_pcm_hw_params_set_buffer_time(): failure");

		/* Feed the parameters back to the device */
		err = snd_pcm_hw_params(sound_device, hw_params);
		if (err < 0)
			throw OutputException("snd_pcm_hw_params_set_channels(): failure");
		snd_pcm_hw_params_free(hw_params);
		hw_params = NULL;

		/* Prepare for lift off! */
		err = snd_pcm_prepare(sound_device);
		if (err < 0)
			throw OutputException("snd_pcm_prepare(): failure");
	} catch (OutputException e) {
		// If we got an error, free the hardware parameters. first.
		if (hw_params != NULL)
			snd_pcm_hw_params_free(hw_params);

		/* Append the ALSA error message to the error */
		throw OutputException(e.what() + string(": ") + string(snd_strerror(err)));
	}
}

void
OutputALSA::play(char* buf, size_t len)
{
	size_t r;

	/*
	 * We are reported bytes, so convert them to ALSA frames
	 * and do not continue until we done playing the buffer.
	 */
	size_t size = snd_pcm_bytes_to_frames(sound_device, len);
	while (size > 0) {
		r = snd_pcm_writei(sound_device, buf, size);
		if (r == -EAGAIN)
			continue;
		if (r == -EPIPE) {
			/*
			 * We get EPIPE if something interrupts ALSA. For example, this occurs
			 * whenever the playing thread gets paused and resumes later on. Just
			 * prepare the device for futher output and be on our merry way.
			 */
			snd_pcm_prepare(sound_device);
			continue;
		}
		// Bail out on any other error
		if (r < 0)
			break;
		buf += r * 4;
		size -= r;
	}
}

OutputALSA::~OutputALSA()
{
	snd_pcm_close(sound_device);
}
