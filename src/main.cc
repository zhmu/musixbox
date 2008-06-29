#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "decode_mp3.h"
#include "decode_tone.h"
#include "input_file.h"
#include "output_ao.h"
#include "visualize_spectrum.h"

InputFile* input;
Output* output;
Decoder* decoder;
Visualizer* vis;

int
main(int argc, char** argv)
{
	output = new OutputAO();
	vis = new SpectrumVisualizer();

	input = new InputFile();
	const char* file = "mp3/music.mp3";
	if (argc == 2)
		file = argv[1];
	if (!input->open(file))
		err(1, "input");

#if 1
	decoder = new DecoderMP3();
#else
	decoder = new DecoderTone();
#endif

	decoder->setInput(input);
	vis->init();
	output->init();

	decoder->setOutput(output);
	decoder->setVisualizer(vis);
	decoder->run();

	output->done();
	vis->done();
	input->close();

	return 0;
}
