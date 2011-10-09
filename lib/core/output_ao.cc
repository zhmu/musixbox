#include <ao/ao.h>
#include <string.h>
#include "exceptions.h"
#include "output_ao.h"

using namespace std;

OutputAO::OutputAO() : Output()
{
	int drvid;
	ao_sample_format format;

	output_dev = NULL;

	ao_initialize();

	drvid = ao_default_driver_id();

	memset(&format, 0, sizeof(format));
	format.bits = 16;
	format.channels = 2;
	format.rate = 44100;
	format.byte_format = AO_FMT_LITTLE;
	output_dev = ao_open_live(drvid, &format, NULL);
	if (output_dev == NULL)
		throw OutputException("ao_open_live() failed to initialize");
}

void
OutputAO::play(char* buf, size_t len)
{
	ao_play(output_dev, buf, len);
}

OutputAO::~OutputAO()
{
	ao_close(output_dev);
	ao_shutdown();
}
