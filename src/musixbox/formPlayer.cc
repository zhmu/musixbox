#include "core/exceptions.h"
#include "ui/form.h"
#include "formPlayer.h"
#include "interface.h"
#include "images.h"

using namespace std;

formPlayer::formPlayer(Interaction* in, Interface* iface)
	: Form(in)
{
	interface = iface;

	lArtist = new Label(0, 0,
	                    interaction->getWidth(), interaction->getTextHeight());
	lAlbum  = new Label(0, 1 * interaction->getTextHeight(),
	                    interaction->getWidth(), interaction->getTextHeight());
	lTitle  = new Label(0, 2 * interaction->getTextHeight(),
	                    interaction->getWidth(), interaction->getTextHeight());
	lPlayingTime = new Label(0, 3 * interaction->getTextHeight(),
	                    interaction->getWidth(), interaction->getTextHeight());
	add(lArtist); add(lAlbum); add(lTitle); add(lPlayingTime);

	bPlay  = new Image( 2, interaction->getHeight() - 12, 8, 8, Images::play());
	bStop  = new Image(14, interaction->getHeight() - 12, 8, 8, Images::stop());
	bPrev  = new Image(26, interaction->getHeight() - 12, 8, 8, Images::prev());
	bNext  = new Image(38, interaction->getHeight() - 12, 8, 8, Images::next());
	bFile  = new Image(50, interaction->getHeight() - 12, 8, 8, Images::file());
	bVolUp = new Image(62, interaction->getHeight() - 12, 8, 8, Images::plus());
	bVolDn = new Image(74, interaction->getHeight() - 12, 8, 8, Images::minus());
	add(bPlay); add(bStop); add(bNext); add(bPrev); add(bFile); add(bVolUp); add(bVolDn);
}

void
formPlayer::update()
{
	Info* info = NULL;
	bool isPaused = false;

	if (interface->getPlayer() != NULL) {
		playingTime = interface->getPlayer()->getPlayingTime();
		totalTime = interface->getPlayer()->getTotalTime();
		info = interface->getPlayer()->getInfo();
		isPaused = interface->getPlayer()->isPaused();
	} else {
		playingTime = 0;
		totalTime = 0;
		info = NULL;
	}

	bPlay->setImage(isPaused ? Images::pause() : Images::play());

	const char* s;
	s = "Unknown Artist";
	if (info != NULL && info->getArtist() != NULL) s = info->getArtist();
	lArtist->setText(s);
	s = "Unknown Album";
	if (info != NULL && info->getAlbum() != NULL) s = info->getAlbum();
	lAlbum->setText(s);
	s = "Unknown Title";
	if (info != NULL && info->getTitle() != NULL) s = info->getTitle();
	lTitle->setText(s);

	char temp[64];
	snprintf(temp, sizeof(temp), "%u:%02u / %u:%02u", playingTime / 60, playingTime % 60, totalTime / 60, totalTime % 60);
	lPlayingTime->setText(temp);
}

void
formPlayer::interact(Control* control)
{
	if (control == bPlay) {
		if (interface->getPlayer() != NULL) {
			/* we are currently playing or paused */
			if (interface->getPlayer()->isPaused()) {
				interface->getPlayer()->cont();
			} else {
				interface->getPlayer()->pause();
			}
		} else {
			interface->playFile();
		}
		return;
	}

	if (control == bStop) {
		if (interface->getPlayer() != NULL)
			interface->getPlayer()->stop();
		return;
	}

	if (control == bNext) {
		interface->next();
		return;
	}

	if (control == bPrev) {
		interface->prev();
		return;
	}

	if (control == bFile) {
		close();
		return;
	}

	if ((control == bVolUp || control == bVolDn) && interface->getMixer() != NULL) {
		try {
			int volume = interface->getMixer()->getVolume();
			if (control == bVolDn) {
				if (volume > 4)
					volume -= 4;
				else
					volume = 0;
			} else {
				volume += 4;
			}
			interface->getMixer()->setVolume(volume);
		} catch (MixerException& e) {
			fprintf(stderr, "Warning: mixer failure: %s", e.what());
		}
		return;
	}
}
