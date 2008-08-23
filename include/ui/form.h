#include <vector>
#include "interaction.h"

class Control;

#ifndef __FORM_H__
#define __FORM_H__

//! \brief Implements a form
class Form {
friend class Control;

public:
	//! \brief Initialize the form
	Form(Interaction* i) {
		interaction = i; mustClose = false;
	};

	//! \brief Deinitialize the form
	virtual ~Form() { };

	/*! \brief Add a control to the form
	 *  \param c The control to be added
	 */
	void add(Control* c);
	
	//! \brief Called to redraw the form;
	virtual void redraw();

	//! \brief Called to process the form
	void run();

	//! \brief Request close of the form
	void close() { mustClose = true; }

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

	//! \brief Does the form has to be closed
	bool mustClose;
};

#endif /* __FORM_H__ */
