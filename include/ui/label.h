#include <string>
#include "ui/control.h"

#ifndef __LABEL_H__
#define __LABEL_H__

class Label : public Control {
public:
	Label(unsigned int x, unsigned int y, unsigned int width, unsigned int height) : Control(x, y, width, height) {
	}

	Label(unsigned int x, unsigned int y, unsigned int width, unsigned int height, std::string text) : Control(x, y, width, height) {
		this->text = text;
	}

	//! \brief Called if the control is to be drawn
	void draw();

	//! \brief Update text on the control
	void setText(std::string s) { text = s; }

	//! \brief Retrieve text form control
	std::string getText() { return text; }

private:
	//! \brief Text to be shown
	std::string text;
};

#endif /* __LABEL_H__ */
