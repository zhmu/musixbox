#include <string>
#include "input.h"
#include "info.h"
#include "output.h"
#include "visualize.h"

#ifndef __DECODERFACTORY_H__
#define __DECODERFACTORY_H__

//! \brief Static class that aids in constructing appropriate decoder and info objects
class DecoderFactory {
public:
	/*! \brief Constructs a decoder, input provider and information object based on a resource
	 *  \param resource Resource to use
	 *  \param output Output object to use
	 *  \param visualizer Visualizer object to use
	 *  \param input Resulting input object
	 *  \param decoder Resulting decoder object
	 *  \param info Resulting info object
	 */
	static void construct(std::string resource, Output* output, Visualizer* visualizer,
	                      Input** input, Decoder** decoder, Info** info);
};

#endif /* __DECODERFACTORY_H__ */
