#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include "decode.h"

#ifndef __DECODE_MIDI_H__
#define __DECODE_MIDI_H__

/*! \brief Handles decoding of MIDIfiles
 *
 *  The decoder used is libSDL_mixer, as it conveniently pulls in Timidity,
 *  which isn't available in a solid library-form otherwise.
 */
class DecoderMIDI : public Decoder {

public:
	DecoderMIDI(Player* p, Input* i, Output* o, Visualizer* v);
	~DecoderMIDI();

	//! \brief Performs a AdLiB decoding run
	void run();

	//! \brief Retrieve a list of file extensions this class can decode
	static std::list<std::string> getExtensions();

private:
	static void postmix_func_wrapper(void* udata, Uint8* stream, int len) {
		DecoderMIDI* decoder = (DecoderMIDI*)udata;
		decoder->postmix_func(stream, len);
	}

	void postmix_func(Uint8* stream, int len);

	//! \brief Wrapper to hook SDL's file operations to our Input
	class RWOps_Wrapper : public SDL_RWops {
	public:
		RWOps_Wrapper(Input* i);

	protected:
		static int seek_func(struct SDL_RWops* context, int offset, int whence);
		static int read_func(struct SDL_RWops* context, void* ptr, int size, int maxnum);
		static int write_func(struct SDL_RWops* context, const void* ptr, int size, int maxnum);
		static int close_func(struct SDL_RWops* context);
	};

	Mix_Music* music;
	RWOps_Wrapper rwops;
};

#endif /* __DECODE_MIDI_H__ */
