#include "ui/form.h"
#include "ui/interaction.h"
#include "formPlaylist.h"
#include "images.h"

using namespace std;

formPlaylist::formPlaylist(Interaction* in, Interface* iface)
	: Form(in)
{
	/*
	 * First of all, introduce the buttons - this ensures that they
	 * override any war between the 'is this a label or a button'-conflict.
	 */
	bUp =    new Image( 0, interaction->getHeight() - 12, 16, 8, Images::up());
	bDown =  new Image(interaction->getWidth() / 2, interaction->getHeight() - 12, 16, 8, Images::down());
	bLeave = new Image(interaction->getWidth() - 16, interaction->getHeight() - 12, 16, 8, Images::leave());
	add(bLeave); add(bUp); add(bDown);

	/* Place the labels on there */
	for (unsigned int i = 0; i < (interaction->getHeight() - interaction->getTextHeight())  / interaction->getTextHeight(); i++) {
		Label* l = new Label(0, i * interaction->getTextHeight(),
		                     interaction->getWidth() - 10, interaction->getTextHeight());
		l->setData(new int(i));
		itemlabel.push_back(l);
		add(l);
	}

	interface = iface; rehash = true; current_index = 0;
}

void
formPlaylist::update()
{
	if (!dirty)
		return;

	/*
	 * Fill the screen until there is no more space.
	 */
	unsigned int cur_label = 0;
	unsigned int max_label = (interaction->getHeight() / interaction->getTextHeight()) - 1;
	unsigned int cur_index = current_index;
	while (cur_index < interface->getPlaylist()->getItems().size()) {
		/* If we have more entries than labels, call it a day */
		if (cur_label == max_label)
			break;

		PlaylistItem* it = interface->getPlaylist()->getItems()[cur_index];
		itemlabel[cur_label]->setText(it->getTitle() + "  - " + it->getArtist() + " / " + it->getAlbum());
		itemlabel[cur_label]->show();
		cur_label++; cur_index++;
	}

	/*
	 * Ensure unused items are properly hidden.
	 */
	while (cur_label < max_label) {
		itemlabel[cur_label]->hide();
		cur_label++;
	}

	/* Show or hide the next/previous buttons */
	bUp->setVisible(current_index > 0);
	bDown->setVisible(cur_index < interface->getPlaylist()->getItems().size());
	rehash = false;
}

void
formPlaylist::interact(Control* control)
{
	/*
	 * If we are rehashing, do not accept any interaction - this prevents
	 * the browser form attemping to navigate two subfolders because it
	 * got two interactions shortly after eachother while the screen was
	 * not yet updated.
	 */
	if (rehash)
		return;

	if (control == bUp || control == bDown) {
		unsigned int screen_size = (interaction->getHeight() / interaction->getTextHeight()) - 1;
		if (control == bUp && current_index >= screen_size) {
			current_index -= screen_size;
			dirty = true;
		}
		if (control == bDown && current_index < interface->getPlaylist()->getItems().size()) {
			current_index += screen_size;
			dirty = true;
		}
		return;
	}

	if (control == bLeave) {
		close();
		return;
	}

	/*
	 * We have a file to play! Start it, and bail out of here - nothing
	 * left for us to do.
	 */
	int i = *(reinterpret_cast<int*>(control->getData()));
	interface->startPlaylist(current_index + i);
	close();
}

int
formPlaylist::run()
{
	dirty = true;
	return Form::run();
}

