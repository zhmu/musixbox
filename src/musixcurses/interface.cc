#include <sys/types.h>
#include <sys/ioctl.h>
#include <curses.h>
#include <signal.h>
#include "curseplayer.h"
#include "interface.h"

using namespace std;

Interface::Interface(Output* o, Mixer* m, Folder* f)
{
	output = o; mixer = m; folder = f; player = NULL; showingPlaylist = false;

	/* Initialize curses and colors */
	initscr();
	if (has_colors() == FALSE) {
		endwin();
		fprintf(stderr, "musixcurses: your terminal does not support colors, exiting\n");
		exit(EXIT_FAILURE);
	}
	start_color();
	init_pair(PAIR_STATUS,  COLOR_YELLOW, COLOR_BLUE);
	init_pair(PAIR_BROWSER, COLOR_WHITE,  COLOR_BLACK);
	init_pair(PAIR_INFO,    COLOR_YELLOW, COLOR_BLUE);

	/* Set our share of curses options, most notably no cursor and key input */
	raw(); cbreak(); keypad(stdscr, TRUE);
	noecho(); curs_set(0); refresh();

	/*
	 * Initialize windows: status window (first 7 lines), browser window (X lines)
	 * and info window (last 7 lines), thus X = total lines - 14.
	 */	
	winStatus  = newwin(7, 0, 0, 0);
	winBrowser = newwin(LINES - 14, 0, 7, 0);
	winInfo    = newwin(7, 0, LINES - 7, 0);

	/* Set colors for all windows */
	wattrset(winStatus, COLOR_PAIR(PAIR_STATUS) | A_BOLD);
	wbkgdset(winStatus, COLOR_PAIR(PAIR_STATUS));
	wattron(winBrowser, COLOR_PAIR(PAIR_BROWSER));
	wattron(winInfo, COLOR_PAIR(PAIR_INFO) | A_BOLD);
	wbkgdset(winInfo, COLOR_PAIR(PAIR_STATUS));

	/* Initialize menu's */
	menuBrowser = new MenuBrowser(winBrowser, folder);
	menuPlaylist = new MenuPlaylist(winBrowser, &playlist);

	/* Dump stuff in the windows we just created and show 'em */
	fillInfo();
	fillStatus();
	menuBrowser->draw();
	refresh();
}

Interface::~Interface()
{
	if (player != NULL) {
		player->stop();
		delete player;
	}

	/* deinitialize curses, this makes our terminal happy again */
	delwin(winInfo); delwin(winBrowser); delwin(winStatus);
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
	mvwprintw(winInfo, 1, 2, "up/dn/pgup/pgdn/home/end   browse                   space  pause/continue");
	mvwprintw(winInfo, 2, 2, "right/enter                select                   +/-    adjust volume");
	mvwprintw(winInfo, 3, 2, "left/backspace             leave folder             f10    exit");
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
Interface::playFile()
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

	p = new CursePlayer(folder->getFullPath(folder->getEntries()[menuBrowser->getSelectedItem()]), output, this);
	player = p;
	player->play();

	fillStatus();
}

void
Interface::trackDone()
{
#if 0
        if (browser_sel_item + 1 >= folder->getEntries().size())
                return;

	browser_sel_item++;
	playFile();
	fillBrowser();
#endif
}

void
Interface::addToPlaylist(string resource)
{
	/* Do not traverse '.' or '..' */
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

	playlist.addItem(new PlaylistItem(resource));
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

	switch(c) {
		case 0x0a: /* ENTER */
		case KEY_RIGHT:
			item = folder->getEntries()[menuBrowser->getSelectedItem()];
			if (item == "..") {
				menuBrowser->reset();
				folder->goUp();
				menuBrowser->draw();
				break;
			}
			if (folder->isFolder(item)) {
				menuBrowser->reset();
				folder->select(item);
				menuBrowser->draw();
				break;
			}
			playFile();
			break;
		case KEY_IC: /* insert */
			addToPlaylist(item);
			break;
		case KEY_LEFT:
			if (folder->getEntries()[0] != "..")
				break;
			menuBrowser->reset();
			folder->goUp();
			menuBrowser->draw();
			break;
		case 0x09: /* TAB */
			menuPlaylist->draw();
			showingPlaylist = true;
			break;
	}
	if (menuBrowser->handleInput(c)) {
		menuBrowser->draw();
		return;
	}
	handleCommonInput(c);
}

void
Interface::handlePlaylistInput(int c)
{
	switch(c) {
		case KEY_DC: /* insert */
			playlist.removeItem(menuPlaylist->getSelectedItem());
			menuPlaylist->draw();
			break;
		case 0x0a: /* ENTER */
			playlist.setCurrentPlayItem(menuPlaylist->getSelectedItem());
			menuPlaylist->draw();
			break;
		case 0x09: /* TAB */
			menuBrowser->draw();
			showingPlaylist = false;
			break;
	}
	if (menuPlaylist->handleInput(c)) {
		menuPlaylist->draw();
		return;
	}
}

void
Interface::run()
{
	/*
	 * Handle input until the user hammers F10.
	 */
	while (1) {
		int c = getch();
		if (c == KEY_F(10))
			break;
		if (showingPlaylist)
			handlePlaylistInput(c);
		else
			handleBrowserInput(c);
	}

}
