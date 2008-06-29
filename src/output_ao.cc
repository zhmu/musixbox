#include <ao/ao.h>
#include "output_ao.h"

int
OutputAO::init()
{
	int drvid;
	ao_sample_format format;

	ao_initialize();

	drvid = ao_default_driver_id();

	format.bits = 16;
	format.channels = 2;
	format.rate = 44100;
	format.byte_format = AO_FMT_LITTLE;
	output_dev = ao_open_live(drvid, &format, NULL);
	if (output_dev == NULL)
		return 0;


	return 1;
}

void
OutputAO::play(char* buf, size_t len)
{
	ao_play(output_dev, buf, len);
}

int
OutputAO::done()
{
	ao_close(output_dev);
	ao_shutdown();
	return 1;
}
