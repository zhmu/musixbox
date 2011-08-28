#include <stdio.h>
#include <stdlib.h>
#include "decode_midi.h"
#include "exceptions.h"

using namespace std;

int
DecoderMIDI::RWOps_Wrapper::seek_func(struct SDL_RWops* context, int offset, int whence)
{
	Input* input = (Input*)context->hidden.unknown.data1;
	off_t new_pos;
	switch(whence) {
		case SEEK_SET:
			new_pos = offset;
			break;
		case SEEK_CUR:
			new_pos = input->getCurrentPosition() + offset;
			break;
		case SEEK_END:
			new_pos = input->getLength() - offset;
			break;
		default:
			return -1;
	}
	if (!input->seek(new_pos))
		return -1;
	return input->getCurrentPosition();
}

int
DecoderMIDI::RWOps_Wrapper::read_func(struct SDL_RWops* context, void* ptr, int size, int maxnum)
{
	Input* input = (Input*)context->hidden.unknown.data1;
	return input->read((char*)ptr, size * maxnum) / size;
}

int
DecoderMIDI::RWOps_Wrapper::write_func(struct SDL_RWops* context, const void* ptr, int size, int maxnum)
{
	return -1;
}

int
DecoderMIDI::RWOps_Wrapper::close_func(struct SDL_RWops* context)
{
	return 0;
}
		
DecoderMIDI::RWOps_Wrapper::RWOps_Wrapper(Input* i)
{
	hidden.unknown.data1 = (void*)i;
	seek = &RWOps_Wrapper::seek_func;
	read = &RWOps_Wrapper::read_func;
	write = &RWOps_Wrapper::write_func;
	close = &RWOps_Wrapper::close_func;
}

DecoderMIDI::DecoderMIDI(Player* p, Input* i, Output* o, Visualizer* v) :
	Decoder(p, i, o, v),
	rwops(i)
{
	/*
	 * This is a very grotesque hack: SDL_mixer requires SDL's audio
	 * structure, even if we only want it to decode files for us, so we'll
	 * use the 'dummy' driver so that it will do the right thing.
	 */
	char* previous_audio_driver = NULL;
	if (SDL_getenv((char*)"SDL_AUDIODRIVER") != NULL)
		previous_audio_driver = strdup(SDL_getenv((char*)"SDL_AUDIODRIVER"));
	else
		previous_audio_driver = strdup("SDL_AUDIODRIVER=");
	SDL_putenv((char*)"SDL_AUDIODRIVER=dummy");

	try {
		if (SDL_Init(SDL_INIT_AUDIO) < 0)
			throw DecoderException(string("DecoderMIDI: cannot initialize SDL: ") + SDL_GetError());
		if (Mix_OpenAudio(44100, AUDIO_S16LSB, 2, 1024) < 0)
			throw DecoderException(string("DecoderMIDI: cannot initialize SDL_mixer: ") + Mix_GetError());
	} catch (...) {
		/* Restore the previous audio driver */
		SDL_putenv(previous_audio_driver);
		free(previous_audio_driver);
		/* And report the error */
		throw;
	}

	/* SDL is all good to go; revert our environment hackery */
	SDL_putenv(previous_audio_driver);
	free(previous_audio_driver);

	/* Throw away any mixing channels, we don't need them */
	Mix_AllocateChannels(0);

	music = Mix_LoadMUS_RW(&rwops);
	if (music == NULL)
		throw DecoderException(string("DecoderMIDI: cannot play this file: ") + Mix_GetError());

	/* Hook up our post-mix function; this will send the samples to our output */
	Mix_SetPostMix(&DecoderMIDI::postmix_func_wrapper, this);

	if (Mix_PlayMusic(music, 1) < 0)
		throw DecoderException(string("DecoderMIDI: cannot play music: ") + Mix_GetError());
	Mix_VolumeMusic(SDL_MIX_MAXVOLUME);
}

DecoderMIDI::~DecoderMIDI()
{
	if (music != NULL)
		Mix_FreeMusic(music);
	Mix_CloseAudio();
}

void
DecoderMIDI::run()
{
	while(!terminating && (Mix_PlayingMusic() || Mix_PausedMusic())) {
		handlePause();
	}
}

void
DecoderMIDI::postmix_func(Uint8* stream, int len)
{
	output->play((char*)stream, len);
}

list<string> 
DecoderMIDI::getExtensions()
{
	list<string> l;
	l.push_back("mid");
	return l;
}
