#include "ui/form.h"
#include "ui/interaction.h"
#include "formBrowser.h"
#include "images.h"

using namespace std;

formBrowser::formBrowser(Interaction* in, Interface* iface, Folder* f)
	: Form(in)
{
	/*
	 * First of all, introduce the buttons - this ensures that they
	 * override any war between the 'is this a label or a button'-conflict.
	 */
	bUp =    new Image( 0, interaction->getHeight() - 12, 16, 16, Images::up());
	bDown =  new Image(interaction->getWidth() / 2, interaction->getHeight() - 12, 16, 16, Images::down());
	bLeave = new Image(interaction->getWidth() - 16, interaction->getHeight() - 12, 16, 16, Images::leave());
	add(bLeave); add(bUp); add(bDown);

	/* Place the labels on there */
	for (unsigned int i = 0; i < (interaction->getHeight() - interaction->getTextHeight())  / interaction->getTextHeight(); i++) {
		Label* l = new Label(10, i * interaction->getTextHeight(),
		                     interaction->getWidth() - 10, interaction->getTextHeight());
		l->setData(new formBrowserControlData(CD_TYPE_LABEL, i));
		dirlabel.push_back(l);

		Image* q = new Image(0, i * interaction->getTextHeight() + 2,
		                     8, interaction->getTextHeight(), Images::miniPlus());
		q->setData(new formBrowserControlData(CD_TYPE_QUEUE, i));
		queuebtn.push_back(q);
		add(q); add(l);
	}

	interface = iface; folder = f; rehash = true; new_visit = true;
	selectedPath = folder->getPath();
}

void
formBrowser::update()
{
	if (!rehash)
		return;

	if (new_visit) {
		/*
		 * We are entering this folder for the first time (i.e. no
		 * next/previous page buttons were touched forcing us to
		 * update). Attempt to look up the visiting page number - if
		 * it's not there, just default to the first one.
		 */
		map<string, unsigned int>::iterator it = cachedIndexMap.find(getIndexKey());

		if(it != cachedIndexMap.end()) {
			current_page = it->second;
		} else {
			current_page  = 0;
		}
		new_visit = false;
	}

	/*
	 * Fill the screen until there is no more space.
	 */
	unsigned int cur_label = 0;
	unsigned int max_label = (interaction->getHeight() / interaction->getTextHeight()) - 1;
	unsigned int cur_index = 0;
	unsigned int skip_num = current_page * max_label;
	nextpage = false;
	while (cur_index < folder->getEntries().size()) {
		/*
		 * If we are in the root folder and we have a filter char, ignore
		 * anything that doesn't match.
		 */
		if (folder->isFolderRoot() && filterChar != '\0' &&
			  folder->getEntries()[cur_index].c_str()[0] != filterChar) {
			cur_index++;
			continue;
		}

		/*
		 * Always skip '..' - it's always possible to go back by
		 * hitting the return button.
		 */
		if (folder->getEntries()[cur_index] == "..") {
			cur_index++;
			continue;
		}

		/*
		 * If we need to skip an entry, as we are trying to visit page
		 * N, do it.
		 */
		if (skip_num > 0) {
			skip_num--; cur_index++;
			continue;
		}

		/*
		 * If there are more entries than we can fit on our screen, we
		 * are done. Note that we set the 'nextpage' flag as it should
		 * be possible to visit the next page.
		 */
		if (cur_label == max_label) {
			nextpage = true;
			break;
		}

		dirlabel[cur_label]->setText(folder->getEntries()[cur_index]);
		dirlabel[cur_label]->show();
		queuebtn[cur_label]->show();
		cur_label++; cur_index++;
	}

	/*
	 * Ensure unused items are properly hidden.
	 */
	while (cur_label < max_label) {
		dirlabel[cur_label]->hide();
		queuebtn[cur_label]->hide();
		cur_label++;
	}

	/* Show or hide the next/previous buttons */
	bUp->setVisible(current_page > 0);
	bDown->setVisible(nextpage);
	rehash = false;
}

void
formBrowser::interact(Control* control)
{
	/*
	 * If we are rehashing, do not accept any interaction - this prevents
	 * the browser form attemping to navigate two subfolders because it
	 * got two interactions shortly after eachother while the screen was
	 * not yet updated.
	 */
	if (rehash)
		return;

	if (control == bLeave) {
		/* Whatever we do, first store the current page */
		cachedIndexMap[getIndexKey()] = current_page;

		/*
		 * If we are in the root folder, exit the dialog - we need the
		 * alphabet browser now.
		 */
		if (folder->isFolderRoot()) {
			selectedFile = "";
			setReturnValue(FORMBROWSER_CODE_GOUP);
			close();
			return;
		}

		/* Simply go one level lower */
		folder->goUp();
		rehash = true; new_visit = true;
		return;
	}

	if (control == bUp || control == bDown) {
		if (control == bDown) {
			current_page++;
		} else {
			current_page--;
		}
		rehash = true;
		return;
	}

	formBrowserControlData* data = (formBrowserControlData*)control->getData();
	if (data->getType() == CD_TYPE_LABEL) {
		/* If we got here, it must have been an item */
		Label* l = reinterpret_cast<Label*>(control);

		/* Rememember on which page we were in the current path */
		cachedIndexMap[getIndexKey()] = current_page;
		if (folder->isFolder(l->getText())) {
			/* It's a path, so enter it */
			folder->select(l->getText());
			rehash = true; new_visit = true;
			return;
		}

		/* We got a file! */
		formBrowserControlData* data = (formBrowserControlData*)l->getData();
		direntry_index = data->getValue();
		selectedPath = folder->getPath();
		selectedFile = folder->getFullPath(l->getText());
		setReturnValue(FORMBROWSER_CODE_SELECTED);
		close();
		return;
	}

	/*
	 * If we got here, it must be an enqueue link; first of all,
	 * update the display so the user knows why we are not
	 * responding...
	 */
	unsigned int num_labels = (interaction->getHeight() / interaction->getTextHeight()) - 1;
	vector<bool> visibleLabels;
	for (unsigned int i = 0; i < num_labels; i++) {
		visibleLabels.push_back(dirlabel[i]->isVisible());
		dirlabel[i]->hide();
		queuebtn[i]->hide();
	}
	unsigned int magic_label = num_labels / 2;
	string stored_label = dirlabel[magic_label]->getText();
	dirlabel[magic_label]->setText("Adding tracks...");
	dirlabel[magic_label]->show();
	redraw();
	interaction->yield();

	/* Enqueue the lot */
	Label* l = (Label*)dirlabel[data->getValue()];
	interface->addToPlaylist(l->getText());

	/* Restore the form */
	for (unsigned int i = 0; i < num_labels; i++) {
		dirlabel[i]->setVisible(visibleLabels[i]);
		queuebtn[i]->setVisible(visibleLabels[i]);
	}
	dirlabel[magic_label]->setText(stored_label);
	redraw();
	interaction->yield();

	/* If we are silent, start playing! */
	if (!interface->havePlayer())
		interface->startPlaylist(0);
}

bool
formBrowser::getNextFile(std::string& file)
{
        /*
	 * Try to ascend through the directory vector to the next file - if
	 * there are no more files, just give up.
         */
        if (++direntry_index >= folder->getEntries().size())
                return false;

	file = selectedPath + "/" + folder->getEntries()[direntry_index];
	return true;
}

bool
formBrowser::getPreviousFile(std::string& file)
{
        if (direntry_index <= 0)
                return false;

	file = selectedPath + "/" + folder->getEntries()[--direntry_index];
	return true;
}

int
formBrowser::run()
{
	rehash = true; new_visit = true;
	return Form::run();
}

std::string
formBrowser::getIndexKey()
{
	if (folder->isFolderRoot())
		return string(1, filterChar);
	return folder->getPath();
}
