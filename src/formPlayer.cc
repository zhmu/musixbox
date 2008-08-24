#include "exceptions.h"
#include "formPlayer.h"
#include "interface.h"

using namespace std;

char playbutton[8]  = { 0xff, 0x7f, 0x3e, 0x1c, 0x08, 0x00, 0x00 ,0x00 };
char pausebutton[8] = { 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00 };
char stopbutton[8]  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
char filebutton[8]  = { 0x40, 0x60, 0x7f, 0x01, 0x01, 0x01, 0xe1, 0x7f };
char nextbutton[8]  = { 0xff, 0x7f, 0x3e, 0x1c, 0x08, 0x7e, 0x00, 0x00 };
char prevbutton[8]  = { 0x00, 0x00, 0x7e, 0x08, 0x1c, 0x3e, 0x7f, 0xff };
char volupbutton[8] = { 0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18 };
char voldnbutton[8] = { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 };

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

	bPlay  = new Image( 2, interaction->getHeight() - 12, 8, 8, playbutton);
	bStop  = new Image(14, interaction->getHeight() - 12, 8, 8, stopbutton);
	bPrev  = new Image(26, interaction->getHeight() - 12, 8, 8, prevbutton);
	bNext  = new Image(38, interaction->getHeight() - 12, 8, 8, nextbutton);
	bFile  = new Image(50, interaction->getHeight() - 12, 8, 8, filebutton);
	bVolUp = new Image(62, interaction->getHeight() - 12, 8, 8, volupbutton);
	bVolDn = new Image(74, interaction->getHeight() - 12, 8, 8, voldnbutton);
	add(bPlay); add(bStop); add(bNext); add(bPrev); add(bFile); add(bVolUp); add(bVolDn);
}

void
formPlayer::update()
{
	Decoder* decoder = interface->getDecoder();
	Info* info = interface->getInfo();

	if (decoder != NULL) {
		playingTime = decoder->getPlayingTime();
		totalTime = decoder->getTotalTime();
	} else {
		playingTime = 0;
		totalTime = 0;
	}

	bPlay->setImage(interface->isPlayerPaused() ? pausebutton : playbutton);

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

	char temp[64 /* XXX */];
	sprintf(temp, "%u:%02u / %u:%02u", playingTime / 60, playingTime % 60, totalTime / 60, totalTime % 60);
	lPlayingTime->setText(temp);
}

void
formPlayer::interact(Control* control)
{
	if (control == bPlay) {
		if (interface->isPlayerThread()) {
			/* we are currently playing or paused */
			if (interface->isPlayerPaused()) {
				interface->cont();
			} else {
				interface->pause();
			}
		} else {
			interface->playFile();
		}
		return;
	}

	if (control == bStop) {
		interface->stop();
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
