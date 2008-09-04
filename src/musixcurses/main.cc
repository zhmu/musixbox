#include "config.h"
#include <curses.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include "core/folder.h"
#include "core/folder_fs.h"
#include "core/output.h"
#ifdef WITH_AO
#include "core/output_ao.h"
#endif
#include "core/player.h"
#include "core/exceptions.h"

#define PAIR_STATUS		1
#define PAIR_BROWSER		2
#define PAIR_INFO		3

using namespace std;

WINDOW* winStatus;
WINDOW* winBrowser;
WINDOW* winInfo;

Folder*		folder;
Output*		output = NULL;
Visualizer*	visualizer = NULL;
Player*		player = NULL;

unsigned int	browser_first_item = 0;
unsigned int	browser_sel_item   = 0;
unsigned int	browser_lines      = 0;

void
fillStatus()
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
fillBrowser()
{
	int x;
	unsigned int line = 0;

	getmaxyx(winBrowser, browser_lines, x);

	/*
	 * Ensure that whatever is selected fits on the screen by adjusting the
	 * scroll position as needed.
	 */
	if (browser_sel_item < browser_first_item) {
		browser_first_item = browser_sel_item - (browser_lines / 2);
	}
	if (browser_sel_item > browser_first_item + browser_lines) {
		browser_first_item = browser_sel_item - (browser_lines / 2);
	}

	/*
	 * Keep placing folder entries on the screen one-by-one until we either
	 * run out of entries or run out of space.
	 */
	werase(winBrowser);
	while (browser_first_item + line < folder->getEntries().size()) {
		if (line >= (unsigned int)browser_lines)
			break;
		if (browser_first_item + line == browser_sel_item)
			wattron(winBrowser, A_REVERSE);
		mvwprintw(winBrowser, line, 1, " %s ",
			folder->getEntries()[browser_first_item + line].c_str());
		if (browser_first_item + line == browser_sel_item)
			wattroff(winBrowser, A_REVERSE);
		line++;
	}
	wrefresh(winBrowser);
}

void
fillInfo()
{
	werase(winInfo);
	box(winInfo, 0, 0);
	mvwprintw(winInfo, 1, 2, "up/dn/pgup/pgdn/home/end   browse                   space  pause/continue");
	mvwprintw(winInfo, 2, 2, "right/enter                select");
	mvwprintw(winInfo, 3, 2, "left/backspace             leave folder             f10    exit");
	wrefresh(winInfo);
}

void
handleUpdate(int n)
{
	fillStatus();
}

void
init()
{
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
	winInfo    = newwin(7,  0, LINES - 7, 0);

	/* Set colors for all windows */
	wattrset(winStatus, COLOR_PAIR(PAIR_STATUS) | A_BOLD);
	wbkgdset(winStatus, COLOR_PAIR(PAIR_STATUS));
	wattron(winBrowser, COLOR_PAIR(PAIR_BROWSER));
	wattron(winInfo, COLOR_PAIR(PAIR_INFO) | A_BOLD);
	wbkgdset(winInfo, COLOR_PAIR(PAIR_STATUS));

	/* Dump stuff in the windows we just created and show 'em */
	fillInfo();
	fillStatus();
	fillBrowser();
	refresh();

	/* Update the status window every second */
	signal(SIGALRM, handleUpdate);
}

void
playFile()
{
	if (player != NULL) {
		player->stop();
		delete player;
	}

	player = new Player(folder->getFullPath(folder->getEntries()[browser_sel_item]), output, visualizer);
	player->play();

	fillStatus();
}

void
handleInput(int c)
{
	string item;

	switch(c) {
		case KEY_UP:
		case KEY_DOWN:
			/* advance menu position */
			if (c == KEY_UP)
				browser_sel_item = ((browser_sel_item > 0) ? browser_sel_item : folder->getEntries().size()) - 1;
			else
				browser_sel_item = (browser_sel_item + 1) % folder->getEntries().size();
			fillBrowser();
			break;
		case 0x0a: /* ENTER */
		case KEY_RIGHT:
			item = folder->getEntries()[browser_sel_item];
			if (item == "..") {
				browser_sel_item = 0; browser_first_item = 0;
				folder->goUp();
				fillBrowser();
				break;
			}
			if (folder->isFolder(item)) {
				browser_sel_item = 0; browser_first_item = 0;
				folder->select(item);
				fillBrowser();
				break;
			}
			playFile();
			break;
		case KEY_LEFT:
			if (folder->getEntries()[0] != "..")
				break;
			browser_sel_item = 0; browser_first_item = 0;
			folder->goUp();
			fillBrowser();
			break;
		case KEY_NPAGE:
			browser_first_item += (browser_lines - 1);
			browser_sel_item += (browser_lines - 1);
			if (browser_sel_item >= folder->getEntries().size())
				browser_sel_item = folder->getEntries().size() - 1;
			fillBrowser();
			break;
		case KEY_PPAGE:
			if (browser_sel_item < (browser_lines - 1)) {
				browser_sel_item = 0;
			} else {
				browser_first_item -= (browser_lines - 1);
				browser_sel_item -= (browser_lines - 1);
			}
			fillBrowser();
			break;
		case KEY_HOME:
			browser_sel_item = 0;
			fillBrowser();
			break;
		case KEY_END:
			browser_sel_item = folder->getEntries().size() - 1;
			fillBrowser();
			break;
		case ' ': /* space */
			if (player != NULL)
				if (player->isPaused())
					player->cont();
				else
					player->pause();
			break;
	}
}

void
cleanup()
{
	/* if music is playing, get rid of it */
	if (player != NULL) {
		player->stop();
		delete player;
	}

	/* deinitialize curses, this makes our terminal happy again */
	delwin(winStatus);
	delwin(winBrowser);
	delwin(winInfo);
	endwin();

	/* nuke the final objects */
	delete output;
	delete folder;
}

void
usage()
{
	fprintf(stderr, "usage: musixcurses [-?h] folder\n\n");
	fprintf(stderr, " -h, -?         this help\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "folder is where your media files are expected to be\n");
	exit(EXIT_SUCCESS);
}

int
main(int argc, char** argv)
{
	int c;

	while ((c = getopt(argc, argv, "?h")) != -1) {
		switch(c) {
			case 'h':
			case '?': usage();
			          /* NOTREACHED */
		}
	}
	argc -= optind;
	argv += optind;
	if (argc < 1) {
		fprintf(stderr, "error: no media path given\n");
		usage();
	}


	folder = new FolderFS(argv[0]);
	output = new OutputAO();

	init();

	/*
	 * Handle input until the user hammers F10.
	 */
	while (1) {
		int c = getch();
		if (c == KEY_F(10))
			break;
		handleInput(c);
	}

	cleanup();

	return EXIT_SUCCESS;
}
