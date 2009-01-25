#include <string>
#include "ui/control.h"
#include "ui/form.h"
#include "ui/interaction.h"

using namespace std;

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

	/* Let the controls draw themselves one by one, if they are visible */
	for (unsigned int i = 0; i < controls.size(); i++) {
		if (controls[i]->isVisible())
			controls[i]->draw();
	}
}

void
Form::putpixel(unsigned int x, unsigned int y, unsigned int c)
{
	interaction->putpixel(x, y, c);
}

void
Form::puttext(unsigned int x, unsigned int y, string s)
{
	interaction->puttext(x, y, s.c_str());
}

void
Form::run()
{
	/*
	 * Ensure we do at least one run of the form - and don't trust lingering
	 * coordinates
	 */
	mustClose = false;
	interaction->flushInteraction();

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

		unsigned int cx, cy, ctype;
		if (!interaction->getInteraction(&cx, &cy, &ctype))
			continue;

		/*
		 * There was interaction somewhere - find the corresponding control
		 * and give it a spin. Note that we only handle the first control
		 * matching.
		 */
		for (unsigned int i = 0; i < controls.size(); i++) {
			if (controls[i]->isVisible() && controls[i]->isTouched(cx, cy)) {
				interact(controls[i]);
				break;
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
