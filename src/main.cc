#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "decode_mp3.h"
#include "output_ao.h"
#include "visualize_spectrum.h"

Output* output;
Decoder* decoder;
Visualizer* vis;

int
main(int argc, char** argv)
{
	output = new OutputAO();
	decoder = new DecoderMP3();
	vis = new SpectrumVisualizer();

	const char* file = "mp3/music.mp3";
	if (argc == 2)
		file = argv[1];
	if (!decoder->open(file))
		err(1, "decode_open");

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
