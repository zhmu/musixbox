#include "ui/form.h"
#include "ui/image.h"
#include "ui/label.h"

#ifndef __FORMPLAYER_H__
#define __FORMPLAYER_H__

class Interface;

class formPlayer : public Form {
public:
	formPlayer(Interaction* in, Interface* iface);

protected:
	void update();
	void interact(Control* control);

private:
	Interface* interface;

	int playingTime, totalTime;

	Label* lArtist;
	Label* lAlbum;
	Label* lTitle;
	Label* lPlayingTime;

	Image* bNext;
	Image* bPlay;
	Image* bFile;
	Image* bPlaylist;
};

#endif /* __FORMPLAYER_H__ */
