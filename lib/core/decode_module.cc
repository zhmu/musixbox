#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mikmod.h>
#include "decode_module.h"
#include "exceptions.h"

/*
 * This is an incredibly dirty hack, but there does not seem to be a cleaner
 * way to do this using the libmikmod API; it was definately not designed
 * with an OO-design in mind :-(
 */
static DecoderModule* DecoderModuleDecoder = NULL;

BOOL
mreader_seek(struct MREADER* mr, long offset, int whence)
{
	Input* input = DecoderModuleDecoder->input;
	switch (whence) {
		case SEEK_CUR: /* seek from current position */
		               offset += input->getCurrentPosition();
		               break;
		case SEEK_END: /* seek from endposition */
		               offset = input->getLength() - offset;
		               break;
		case SEEK_SET: /* go there, now */
		               break;
		      default: /* ??? */
		               return -1;
		               
	}
	return input->seek(offset) ? 1 : 0;
}

long
mreader_tell(struct MREADER* mr)
{
	return DecoderModuleDecoder->input->getCurrentPosition();
}

BOOL
mreader_read(struct MREADER* mr, void* dest, size_t length)
{
	return DecoderModuleDecoder->input->read((char*)dest, length) ? 1 : 0;
}

int
mreader_get(struct MREADER* mr)
{
	int i;
	if (!mreader_read(mr, &i, 1))
		return EOF;
	return i;
}

BOOL
mreader_eof(struct MREADER* mr)
{
	return (DecoderModuleDecoder->input->getCurrentPosition() >=
	        DecoderModuleDecoder->input->getLength()) ? 1 : 0;
}

/*
 * mreader_input_wrapper is responsible for handing our Input stream to
 * libmikmod.
 */
struct MREADER mreader_input_wrapper = {
	mreader_seek,
	mreader_tell,
	mreader_read,
	mreader_get,
	mreader_eof
};

static BOOL
MBX_IsThere()
{
	return 1;
}

static BOOL
MBX_Init()
{
	md_mode |= DMODE_SOFT_MUSIC | DMODE_SOFT_SNDFX | DMODE_16BITS | DMODE_STEREO;
	return VC_Init();
}

static void
MBX_Exit()
{
	VC_Exit();
}

void
MBX_Update()
{
	char* buf = DecoderModuleDecoder->getBuffer();
	unsigned int len = VC_WriteBytes((SBYTE*)buf, DECODER_MODULE_BUFFER_SIZE);

	if (DecoderModuleDecoder->visualizer != NULL)
		DecoderModuleDecoder->visualizer->update(buf, len);
	DecoderModuleDecoder->output->play(buf, len);
		
}

/*
 * mdriver_output_wrapper is reponsible for handing decoded audio to the Output
 * plugin.
 */
MDRIVER mdriver_output_wrapper = {
	NULL,
	(CHAR*)"Musixbox output driver",
	(CHAR*)"Musixbox output driver v1.0",
	0, 255,
	(CHAR*)"mbx",
	NULL,
	MBX_IsThere,
	VC_SampleLoad,
	VC_SampleUnload,
	VC_SampleSpace,
	VC_SampleLength,
	MBX_Init,
	MBX_Exit,
	NULL,
	VC_SetNumVoices,
	VC_PlayStart,
	VC_PlayStop,
	MBX_Update,
	NULL,
	VC_VoiceSetVolume,
	VC_VoiceGetVolume,
	VC_VoiceSetFrequency,
	VC_VoiceGetFrequency,
	VC_VoiceSetPanning,
	VC_VoiceGetPanning,
	VC_VoicePlay,
	VC_VoiceStop,
	VC_VoiceStopped,
	VC_VoiceGetPosition,
	VC_VoiceRealVolume
};

DecoderModule::DecoderModule(Input* i, Output* o, Visualizer* v) :
	Decoder(i, o, v)
{
	static int _init = 0;

	DecoderModuleDecoder = this;

	/*
	 * Initialize Mikmod's loaders, and force it to use our output wrapper (as we do not
	 * register any other outputs.
	 *
	 * For some reason, libMikMod seems to get confused if we do this multiple times;
	 * to this avail, ensure this is done exactly one time and no more. The problem seems
	 * to be in the loader chain becoming a ring...
	 */
	if (!_init++) {
		MikMod_RegisterAllLoaders();
		MikMod_RegisterDriver(&mdriver_output_wrapper);
	}

	buffer = (char*)malloc(DECODER_MODULE_BUFFER_SIZE);
	if (buffer == NULL)
		throw DecoderException(std::string("DecoderModule: out of memory"));

	if (MikMod_Init(""))
		throw DecoderException(std::string("DecoderModule: unable to initialize MikMod library: ") + MikMod_strerror(MikMod_errno));

	/* Load the module */
	module = Player_LoadGeneric(&mreader_input_wrapper, 64, 0);
	if (module == NULL)
		throw DecoderException(std::string("DecoderModule: unable to load module"));
}

DecoderModule::~DecoderModule()
{
	MikMod_Exit();

	if (buffer != NULL)
		free(buffer);

	DecoderModuleDecoder = NULL;
}

int
DecoderModule::run()
{
	Player_Start(module);
	while (!terminating && Player_Active()) {
		MikMod_Update();

		playingtime = module->sngtime / 1024;
		usleep(500);
	}

	Player_Stop();
	
	return 0;
}
