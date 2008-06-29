#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "decode_mp3.h"
#include "decode_ogg.h"
#include "decode_tone.h"
#include "input_file.h"
#include "interface.h"
#include "interaction_sdl.h"
#include "output_ao.h"
#include "output_null.h"
#include "visualize_spectrum.h"

InputFile* input;
Output* output;
Decoder* decoder;
Visualizer* vis;
Interaction* interaction;
Interface* interface;

int
main(int argc, char** argv)
{
	//output = new OutputAO();
	output = new OutputNull();
	vis = new SpectrumVisualizer();

	input = new InputFile();
	const char* file = "mp3/music.mp3";
	if (argc == 2)
		file = argv[1];
	if (!input->open(file))
		err(1, "input");

	interaction = new InteractionSDL();
	interface = new Interface(interaction);

/*
	decoder = new DecoderMP3();

	decoder->setInput(input);
	vis->init();
	output->init();

	decoder->setOutput(output);
	decoder->setVisualizer(vis);
	decoder->run();

	output->done();
	vis->done();
	input->close();
*/

	interaction->init();
	interface->init();

	interface->run();

	interaction->done();
	
	return 0;
}
