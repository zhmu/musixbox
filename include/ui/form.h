#include <string>
#include <vector>

class Control;
class Interaction;

#ifndef __FORM_H__
#define __FORM_H__

//! \brief Implements a form
class Form {
friend class Control;

public:
	//! \brief Initialize the form
	Form(Interaction* i) {
		interaction = i; mustClose = false; return_value = 0;
	};

	//! \brief Deinitialize the form
	virtual ~Form() { };

	/*! \brief Add a control to the form
	 *  \param c The control to be added
	 */
	void add(Control* c);
	
	//! \brief Called to redraw the form;
	virtual void redraw();

	/*! \brief Called to process the form
	 *  \returns The return code as set using setReturnValue()
	 *
	 *  Calling this function will reset the return code to zero.
	 */
	virtual int run();

	//! \brief Request close of the form
	void close() { mustClose = true; }

	//! \brief Sets the return value
	void setReturnValue(int i) { return_value = i; }

	//! \brief Does the form has to be closed?
	bool mustBeClosed() { return mustClose; };

protected:
	/*! \brief Place a pixel on the form
	 *  \param x X-position
	 *  \param y Y-position
	 *  \param c Color
	 */
	void putpixel(unsigned int x, unsigned int y, unsigned int c);

	/*! \brief Place text on the screen
	 *  \param x X-position
	 *  \param y Y-position
	 *  \param s String to place
	 */
	void puttext(unsigned int x, unsigned int y, std::string s);

	/*! \brief Called if a control is being interacted
	 *  \parm control Control that is interacted with
	 */
	virtual void interact(Control* control);

	//! \brief Called if the form could be updated
	virtual void update() { };

	//! \brief Interaction provider
	Interaction* interaction;

private:
	//! \brief Controls on the form
	std::vector<Control*> controls;

	//! \brief Return value of the run() function
	int return_value;
	
	//! \brief Does the form has to be closed
	bool mustClose;
};

#endif /* __FORM_H__ */
