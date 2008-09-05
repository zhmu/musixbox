#include <list>
#include <string>
#include "mixer.h"
#include "output.h"

#ifndef __OUTPUTMIXERFACTORY_H__
#define __OUTPUTMIXERFACTORY_H__

//! \brief Static class that aids in constructing a compitable output/mixer combination
class OutputMixerFactory {
public:
	/*! \brief Constructs  output and mixer objects
	 *  \param resource Resource to use
	 *  \param output Resulting output object
	 */
	static void construct(std::string resource, Output** output, Mixer** mixer);

	//! \brief Retrieve a list of available outputs
	static void getAvailable(std::list<std::string>& o);
};

#endif /* __OUTPUTMIXERFACTORY_H__ */
