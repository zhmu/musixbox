#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "decode_mp3.h"
#include "decode_tone.h"
#include "output_ao.h"
#include "visualize_spectrum.h"

Output* output;
Decoder* decoder;
Visualizer* vis;

int
main(int argc, char** argv)
{
	output = new OutputAO();
	vis = new SpectrumVisualizer();

#if 1
	decoder = new DecoderMP3();
	const char* file = "mp3/music.mp3";
	if (argc == 2)
		file = argv[1];
	if (!decoder->open(file))
		err(1, "decode_open");
#else
	decoder = new DecoderTone();
#endif

	vis->init();
	output->init();

	decoder->setOutput(output);
	decoder->setVisualizer(vis);
	decoder->run();

	decoder->close();
	output->done();
	vis->done();

	return 0;
}
