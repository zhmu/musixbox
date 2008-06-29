#include <string>
#include "interaction.h"

#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#define INTERFACE_BROWSER_BAR_SIZE	8

//! \brief Provides user interaction
class Interface {
public:
	//! \brief Constructs a new interface object
	Interface(Interaction* i) {
		interaction = i;
	}

	/*! \brief Initialize interface provider
	 *  \return Non-zero on success
	 */
	int init();

	//! \brief Run the interface 
	void run();

	//! \brief Deinitialize interaction provider
	void done();

protected:
	/*! \brief Update display for file/directory browser
	 *  \returns File to play, or empty string on error/cancel
	 */
	std::string launchBrowser();

	//! \brief Launch the player
	void launchPlayer();

private:
	Interaction* interaction;

	//! \brief Current path of the browser
	std::string currentPath;
};

#endif /* __INTERFACE_H__ */
