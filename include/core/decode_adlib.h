#include <adplug/adplug.h>
#include <adplug/emuopl.h>
#include <adplug/fprovide.h>
#include <libbinio/binstr.h>
#include "decode.h"

#ifndef __DECODE_ADLIB_H__
#define __DECODE_ADLIB_H__

/*! \brief Handles decoding of AdLib files
 *
 *  The decoder used is libadplug
 */
class DecoderAdLib : public Decoder {
	friend class InfoAdLib;

public:
	DecoderAdLib(Player* p, Input* i, Output* o, Visualizer* v);
	~DecoderAdLib();

	//! \brief Performs a AdLiB decoding run
	void run();

	//! \brief Retrieve a list of file extensions this class can decode
	static std::list<std::string> getExtensions();

protected:
	//! \brief Helper class used for our file stream
	class FileProviderHelper : public CFileProvider {
	public:
		FileProviderHelper(Input* i);
		virtual ~FileProviderHelper();

		virtual binistream* open(std::string filename) const { return stream; }
		virtual void close(binistream* f) const { }

	private:
		binisstream* stream;
		void* stream_buffer;
	};

	//! \brief Retrieves the player object
	CPlayer* getPlayer() { return adplayer; }

private:
	FileProviderHelper* fileProvider;
	CEmuopl  opl;
	CPlayer* adplayer;
	char* play_buffer;
	short* opl_buffer;
};


#endif /* __DECODE_ADLIB_H__ */
