#include <mikmod.h>
#include "decode.h"

#ifndef __DECODE_MODULE_H__
#define __DECODE_MODULE_H__

#define DECODER_MODULE_BUFFER_SIZE	8192

/*! \brief Handles decoding of Amiga Module files
 *
 *  The decoder used is libmikmod.
 */
class DecoderModule : public Decoder {
	friend BOOL mreader_seek(struct MREADER* mr, long offset, int whence);
	friend long mreader_tell(struct MREADER* mr);
	friend BOOL mreader_read(struct MREADER* mr, void* dest, size_t length);
	friend int mreader_get(struct MREADER* mr);
	friend BOOL mreader_eof(struct MREADER* mr);
	friend void MBX_Update();

public:
	DecoderModule(Input* i, Output* o, Visualizer* v);
	~DecoderModule();

	//! \brief Performs a module decoding run
	void run();

protected:
	//! \brief Retrieve the temporary decoding buffer
	inline char* getBuffer() { return buffer; }

	//! \brief Retrieve the module object
	inline MODULE* getModule() { return module; }

private:
	//! \brief Module being played
	MODULE* module;

	//! \brief Temporary decoding buffer
	char* buffer;
};

#endif /* __DECODE_MODULE_H__ */
