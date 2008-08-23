#include "form.h"

#ifndef __CONTROL_H__
#define __CONTROL_H__

//! \brief Implements an abstract control
class Control {
	friend class Form;

public:
	/*! \brief Initialize the control
	 *  \param x X position of the control
	 *  \param y Y position of the control
	 *  \param width Width of the control
	 *  \param height Height of the control
	 */
	Control(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

	//! \brief Deinitialize the control
	virtual ~Control() { };

	//! \brief Retrieve control X position
	unsigned int getX() { return x; };

	//! \brief Retrieve control Y position
	unsigned int getY() { return y; };

	//! \brief Retrieve control height
	unsigned int getHeight() { return height; };

	//! \brief Retrieve control width
	unsigned int getWidth() { return width; };

	//! \brief Called if the control is to be drawn
	virtual void draw() = 0;

	//! \brief Is the control touched by the given coordinates
	virtual bool isTouched(unsigned int cx, unsigned int cy);

	//! \brief Called if the control is being interacted with
	virtual void interact() { }

	//! \brief Set private data
	void setData(void* ptr) { data = ptr; }

	//! b\rief Retrieve data
	void* getData() { return data; }

protected:
	//! \brief X and Y position
	unsigned int x, y;

	//! \brief height and width
	unsigned int height, width;

	//! \brief Bind the control to a form
	void setForm(Form* f) { form = f; }

	//! \brief Form we belong to
	Form* form;

	//! \brief Control's private data;
	void* data;

	//! \brief Place a pixel on the screen
	virtual void putpixel(unsigned int x, unsigned int y, unsigned int c);

	//! \brief Place text on the screen
	virtual void puttext(unsigned int x, unsigned int y, std::string s);
};

#endif /* __CONTROL_H__ */
