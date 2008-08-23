#include "ui/control.h"

#ifndef __IMAGE_H__
#define __IMAGE_H__

//! \brief Implements a image
class Image : public Control {
public:
	Image(unsigned int x, unsigned int y, unsigned int width, unsigned int height) : Control(x, y, width, height) {
		image = NULL;
	}

	Image(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const char* img) : Control(x, y, width, height) {
		image = img;
	}

	//! \brief Called if the control is to be drawn
	void draw();

	//! \brief Update text on the control
	void setImage(const char* img) { image = img; }

private:
	//! \brief Image to be drawn
	const char* image;
};

#endif /* __IMAGE_H__ */
