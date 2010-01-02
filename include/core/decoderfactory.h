#include <list>
#include <string>
#include "input.h"
#include "info.h"
#include "output.h"
#include "visualize.h"

#ifndef __DECODERFACTORY_H__
#define __DECODERFACTORY_H__

//! \brief Static class that aids in constructing appropriate decoder and info objects
class DecoderFactory {
	friend class InfoFactory;
public:
	/*! \brief Constructs a decoder, input provider and information object based on a resource
	 *  \param resource Resource to use
	 *  \param player Assorted player object
	 *  \param output Output object to use
	 *  \param visualizer Visualizer object to use
	 *  \param input Resulting input object
	 *  \param decoder Resulting decoder object
	 *  \param info Resulting info object
	 */
	static void construct(std::string resource, Player* player, Output* output,
	                      Visualizer* visualizer, Input** input, Decoder** decoder,
	                      Info** info);

	/*! \brief Retrieves a list of all extensions that can be handled
	 *
	 *  This is useful for applications implementing a media library and
	 *  thus need to guess which files are worth scanning/
	 */
	static void getExtensions(std::list<std::string>& extensions);

protected:
	/*! \brief Check whether an extension can be handled by a decodea
	 *  \param extensions List of extensions
	 *  \param ext Extension to check
	 *  \returns true if it can be handeled
	 */
	static bool checkExtension(std::list<std::string> extensions, std::string ext);
};

#endif /* __DECODERFACTORY_H__ */
