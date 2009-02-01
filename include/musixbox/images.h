#include <stdint.h>

#ifndef __IMAGES_H__
#define __IMAGES_H__

class Images {
public:
	//! \brief Returns an arrow-up image
	static const char* up();

	//! \brief Returns an arrow-down image
	static const char* down();

	//! \brief Returns a leave image
	static const char* leave();

	//! \brief Returns a play |> image
	static const char* play();

	//! \brief Returns a pause || image
	static const char* pause();

	//! \brief Returns a stop [] image
	static const char* stop();

	//! \brief Returns a file image (music note)
	static const char* file();

	//! \brief Returns a next |>| icon
	static const char* next();

	//! \brief Returns a previous |<| icon
	static const char* prev();

	//! \brief Returns a +
	static const char* plus();

	//! \brief Returns a -
	static const char* minus();
};

#endif /* __IMAGES_H__ */
