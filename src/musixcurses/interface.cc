#include <sys/types.h>
#include <sys/ioctl.h>
#include <curses.h>
#include <signal.h>
#include "core/exceptions.h"
#include "curseplayer.h"
#include "interface.h"

using namespace std;

Interface::Interface(Output* o, Mixer* m, Folder* f, const char* resource)
{
	output = o; mixer = m; folder = f; player = NULL;
	showingPlaylist = false; showHelp = true; winMsg = NULL;

	/* Initialize curses and colors */
	initscr();
	start_color();
	init_pair(PAIR_STATUS,  COLOR_YELLOW, COLOR_BLUE);
	init_pair(PAIR_BROWSER, COLOR_WHITE,  COLOR_BLACK);
	init_pair(PAIR_INFO,    COLOR_YELLOW, COLOR_BLUE);
	init_pair(PAIR_DIALOG,  COLOR_BLACK,  COLOR_WHITE);

	/* Set our share of curses options, most notably no cursor and key input */
	raw(); cbreak(); keypad(stdscr, TRUE);
	noecho(); curs_set(0); refresh();

	/*
	 * Call reposition for the first time to reposition all windows - but
	 * initialize the windows to NULL to prevent garbage from being
	 * freed.
	 */
	winStatus = NULL; winBrowser = NULL; winBrowser = NULL;
	reposition();

	/* Initialize menu's */
	menuBrowser = new MenuBrowser(winBrowser, folder);
	menuPlaylist = new MenuPlaylist(winBrowser, &playlist);

	/* Dump stuff in the windows we just created and show 'em */
	redraw();

	if (resource == NULL)
		return;

	try {
		playResource(resource);
	} catch (MusixBoxException& e) {
		/* Unable to play the resource - inform the user */
		string s = string("unable to play initial file: ") + e.what();
		dialog(s);
	}
}

Interface::~Interface()
{
	if (player != NULL) {
		player->stop();
		delete player;
	}

	/* deinitialize curses, this makes our terminal happy again */
	delwin(winInfo); delwin(winBrowser); delwin(winStatus);
	if (winMsg != NULL)
		delwin(winMsg);
	endwin();
}

void
Interface::fillStatus()
{
	unsigned int playingTime = 0, totalTime = 0;
	const char* s;
	char temp[64];
	Info* info = NULL;

	if (player != NULL) {
		playingTime = player->getPlayingTime();
		totalTime = player->getTotalTime();
		info = player->getInfo();
	}

	/*
	 * Nuke everything in the status window and put the new stuff in
	 * there.
	 */
	werase(winStatus);
	box(winStatus, 0, 0);
	s = "Unknown Artist";
	if (info != NULL && info->getArtist() != NULL) s = info->getArtist();
	mvwprintw(winStatus, 1, 2, s);
	s = "Unknown Album";
	if (info != NULL && info->getAlbum() != NULL) s = info->getAlbum();
	mvwprintw(winStatus, 2, 2, s);
	s = "Unknown Title";
	if (info != NULL && info->getTitle() != NULL) s = info->getTitle();
	mvwprintw(winStatus, 3, 2, s);

	snprintf(temp, sizeof(temp), "%u:%02u / %u:%02u", playingTime / 60, playingTime % 60, totalTime / 60, totalTime % 60);
	mvwprintw(winStatus, 5, 2, temp);

	/* Force an update; the alarm function ensures we trigger an update about a second later */
	wrefresh(winStatus);
	alarm(1);
}

void
Interface::fillInfo()
{
	werase(winInfo);
	box(winInfo, 0, 0);
	mvwprintw(winInfo, 1, 2, "up/dn/pgup/pgdn/home/end   navigate                   space  pause/continue");
	mvwprintw(winInfo, 2, 2, "right/enter                select                     +/-    adjust volume");
	mvwprintw(winInfo, 3, 2, "left/backspace             leave folder               f10    exit");
	mvwprintw(winInfo, 4, 2, "insert/delete              add/remove from playlist   *      clear playlist");
	mvwprintw(winInfo, 5, 2, "tab                        toggle browser/playlist    f1     toggle help");
	wrefresh(winInfo);
}

void
Interface::requestUpdate()
{
	fillStatus();
}

void
Interface::signalResize()
{

	struct winsize ws;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
		resizeterm(ws.ws_row, ws.ws_col);
		wrefresh(curscr);
	}
}

void
Interface::playResource(string resource)
{
	/*
	 * We have to be careful here; it is always possible that the player
	 * object is referenced during a signal. To that end, ensure the
	 * player object can't be referenced before we tamper with it, this
	 * closes any races.
	 */
	Player* p = player;
	player = NULL;
	if (p != NULL) {
		p->stop();
		delete p;
	}

	p = new CursePlayer(resource, output, this);
	player = p;
	player->play();

	fillStatus();
}

void
Interface::trackDone()
{
	/*
	 * If we are not playing from the playlist, do nothing. This gets
	 * called whenever the current track is done, so we need to advance
	 * only the playlist.
	 */
	if (!playingFromList)
		return;

	try {
		string item = playlist.getNextResource();
		if (item != "")
			playResource(item);
	} catch (MusixBoxException& e) {
		/* Something was unhappy - skip the track */
		trackDone();
	}

	/*
	 * If we are showing the playlist, update it so that the playing
	 * track is correctly marked.
	 */
	if (showingPlaylist)
		menuPlaylist->draw();
}

void
Interface::addToPlaylist(string resource)
{
	if (resource == "." || resource == "..")
		return;

	/* If the resource is a folder, enter it and recursively add */
	if (folder->isFolder(resource)) {
		folder->select(resource);
		for (unsigned int i = 0; i < folder->getEntries().size(); i++)
			addToPlaylist(folder->getEntries()[i]);
		folder->goUp();
		return;
	}

	/* Upon adding, resolve the complete path */
	try {
		playlist.addItem(new PlaylistItem(folder->getFullPath(resource)));
	} catch (DecoderException& e) {
		/* Something made the decoder unhappy - just skip the file */
	}
}

void
Interface::removeFromPlaylist(string resource)
{
	if (resource == "." || resource == "..")
		return;

	/* If the resource is a folder, enter it and recursively remove */
	if (folder->isFolder(resource)) {
		folder->select(resource);
		for (unsigned int i = 0; i < folder->getEntries().size(); i++)
			removeFromPlaylist(folder->getEntries()[i]);
		folder->goUp();
		return;
	}

	/* Get rid of the entire item */
	playlist.removeItem(folder->getFullPath(resource));
}

void
Interface::handleCommonInput(int c)
{
	int vol;

	switch(c) {
		case ' ': /* space */
			if (player != NULL)
				if (player->isPaused())
					player->cont();
				else
					player->pause();
			break;
		case '+':
		case '-':
		case '=':
			if (mixer == NULL)
				break;
			vol = mixer->getVolume();
			if (c == '-')
				vol = (vol > 4) ? vol - 4 : 0;
			else
				vol = vol + 4;
			mixer->setVolume(vol);
			break;
	}
}

void
Interface::handleBrowserInput(int c)
{
	string item;

	if (menuBrowser->handleInput(c)) {
		menuBrowser->draw();
		return;
	}

	switch(c) {
		case 0x0a: /* ENTER */
		case KEY_RIGHT:
			item = folder->getEntries()[menuBrowser->getSelectedItem()];
			if (item == "..") {
				menuBrowser->storePosition();
				menuBrowser->reset();
				folder->goUp();
				menuBrowser->attemptReturnPosition();
				menuBrowser->draw();
				break;
			}
			if (folder->isFolder(item)) {

				menuBrowser->storePosition();
				menuBrowser->reset();
				folder->select(item);
				menuBrowser->attemptReturnPosition();
				menuBrowser->draw();
				break;
			}
			playingFromList = false;
			try {
				playResource(folder->getFullPath(folder->getEntries()[menuBrowser->getSelectedItem()]));
			} catch (MusixBoxException& e) {
				/* Something went wrong */
			}
			break;
		case KEY_IC: /* insert */
			item = folder->getEntries()[menuBrowser->getSelectedItem()];
			drawDialog("Adding files to playlist...");
			addToPlaylist(item);
			clearDialog();
			break;
		case KEY_DC: /* delete */
			item = folder->getEntries()[menuBrowser->getSelectedItem()];
			removeFromPlaylist(item);
			break;
		case KEY_LEFT:
			if (folder->getEntries()[0] != "..")
				break;
			menuBrowser->reset();
			folder->goUp();
			menuBrowser->attemptReturnPosition();
			menuBrowser->draw();
			break;
		case 0x09: /* TAB */
			menuPlaylist->draw();
			showingPlaylist = true;
			break;
	}
	handleCommonInput(c);
}

void
Interface::handlePlaylistInput(int c)
{
	string item;

	if (menuPlaylist->handleInput(c)) {
		menuPlaylist->draw();
		return;
	}
	switch(c) {
		case KEY_DC: /* delete */
			playlist.removeItem(menuPlaylist->getSelectedItem());
			menuPlaylist->draw();
			break;
		case 0x0a: /* ENTER */
			playlist.setCurrentPlayItem(menuPlaylist->getSelectedItem());
			item = playlist.getCurrentResource();
			if (item != "") {
				playingFromList = true;
				try {
					playResource(item);
				} catch (MusixBoxException& e) {
					/* Something went wrong - skip the track */
					trackDone();
				}
			}
			menuPlaylist->draw();
			break;
		case '*':
			playlist.clear();
			menuPlaylist->draw();
			break;
		case 0x09: /* TAB */
			menuBrowser->draw();
			showingPlaylist = false;
			break;
	}
	handleCommonInput(c);
}

void
Interface::run()
{
	/*
	 * Handle input until the user hammers F10.
	 */
	while (1) {
		int c = getch();
		if (c == KEY_F(1)) {
			/*
			 * Toggle help - and redraw about everything
			 * to ensure it's immediately visibleely visible
			 */
			showHelp = !showHelp;
			reposition();
			redraw();
			continue;
		}
		if (c == KEY_F(10))
			break;
		if (showingPlaylist)
			handlePlaylistInput(c);
		else
			handleBrowserInput(c);
	}

}

void
Interface::reposition()
{
	/* Get rid of the old windows first */
	if (winStatus != NULL) delwin(winStatus);
	if (winBrowser != NULL) delwin(winBrowser);
	if (winInfo != NULL) delwin(winInfo);

	/*
	 * Initialize windows: status window (first 7 lines), browser window (X
	 * lines) and info window (last 7 lines), thus X = total lines - 14.
	 *
	 * Note: if the help is hidden, the info window's doesn't exist.
	 */	
	winStatus  = newwin(7, 0, 0, 0);
	winBrowser = newwin(LINES - (showHelp ? 14 : 7), 0, 7, 0);
	if (showHelp)
		winInfo = newwin(7, 0, LINES - 7, 0);

	/* Set colors for all windows */
	wattrset(winStatus, COLOR_PAIR(PAIR_STATUS) | A_BOLD);
	wbkgdset(winStatus, COLOR_PAIR(PAIR_STATUS));
	wattron(winBrowser, COLOR_PAIR(PAIR_BROWSER));
	if (showHelp) {
		wattron(winInfo, COLOR_PAIR(PAIR_INFO) | A_BOLD);
		wbkgdset(winInfo, COLOR_PAIR(PAIR_STATUS));
		fillInfo();
	}
}

void
Interface::redraw()
{
	fillStatus();
	if (showingPlaylist)
		menuPlaylist->draw();
	else
		menuBrowser->draw();
	refresh();
}

void
Interface::drawDialog(std::string s, bool keymsg)
{
	string keystr = string("<any key to continue>");

	if (winMsg != NULL)
		delwin(winMsg);

	/* Figure out the dialog size */
	unsigned int width = s.size();
	if (keymsg && width < keystr.size())
		width = keystr.size();

	winMsg = newwin(5, 10 + width, (LINES - 5) / 2, (COLS - 10 - s.size()) / 2);
	wattron(winMsg, COLOR_PAIR(PAIR_DIALOG));
	wbkgdset(winMsg, COLOR_PAIR(PAIR_DIALOG));
	
	werase(winMsg);
	box(winMsg, 0, 0);
	mvwprintw(winMsg, 2, 5 + (width - s.size()), s.c_str());
	if (keymsg)
		mvwprintw(winMsg, 4, (width - 10) / 2, keystr.c_str());

	wrefresh(winMsg);
	refresh();
	
}

void
Interface::clearDialog()
{
	if (winMsg != NULL)
		delwin(winMsg);
	redraw();
}

void
Interface::dialog(std::string s)
{
	drawDialog(s, true);
	(void)getch();
	clearDialog();
}
