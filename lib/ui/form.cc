#include <string>
#include "ui/control.h"
#include "ui/form.h"

void
Form::add(Control* c)
{
	c->setForm(this);
	controls.push_back(c);
}

void
Form::redraw()
{
	/* Clear everything */
       interaction->clear(0, 0, interaction->getHeight(), interaction->getWidth());

	/* Let the controls draw themselves one by one */
	for (unsigned int i = 0; i < controls.size(); i++) {
		controls[i]->draw();
	}
}

void
Form::putpixel(unsigned int x, unsigned int y, unsigned int c)
{
	interaction->putpixel(x, y, c);
}

void
Form::puttext(unsigned int x, unsigned int y, std::string s)
{
	interaction->puttext(x, y, s.c_str());
}

void
Form::run()
{
	/* Ensure we do at least one run of the form */
	mustClose = false;

	while (!mustBeClosed()) {
		/* Update form and throw it on the interaction provider */
		update();
		redraw();
		interaction->yield();

		/* If the interaction provider wants to terminate, so should we */
		if (interaction->mustTerminate()) {
			close();
			continue;
		}

		unsigned int cx, cy;
		if (!interaction->getCoordinates(&cx, &cy))
			continue;

		/*
		 * There was interaction somewhere - find the corresponding control
		 * and give it a spin
		 */
		for (unsigned int i = 0; i < controls.size(); i++) {
			if (controls[i]->isTouched(cx, cy)) {
				interact(controls[i]);
			}
		}
	}
}

void
Form::interact(Control* control)
{
	if (control != NULL)
		control->interact();
}