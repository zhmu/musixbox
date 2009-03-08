#include "core/exceptions.h"
#include "ui/font.h"
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
	lArtist->setFont(&font8); lAlbum->setFont(&font8); lTitle->setFont(&font8); lPlayingTime->setFont(&font8);
	add(lArtist); add(lAlbum); add(lTitle); add(lPlayingTime);

	bPlay     = new Image( 0, interaction->getHeight() - 12, 24, 8, Images::play());
	bNext     = new Image(24, interaction->getHeight() - 12, 24, 8, Images::next());
	bFile     = new Image(48, interaction->getHeight() - 12, 24, 8, Images::file());
	bPlaylist = new Image(72, interaction->getHeight() - 12, 24, 8, Images::playlist());
	add(bPlay); add(bNext); add(bFile);
	add(bPlaylist);
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

	bPlay->setImage(isPaused ? Images::play() : Images::pause());

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
			interface->playResource(interface->getCurrentFile());
		}
		return;
	}

	if (control == bNext) {
		interface->next();
		return;
	}

	if (control == bFile) {
		setReturnValue(0);
		close();
		return;
	}

	if (control == bPlaylist) {
		setReturnValue(1);
		close();
		return;
	}
}
