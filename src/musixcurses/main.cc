#include "config.h"
#include <curses.h>
#include <pthread.h>
#include <pthread_np.h>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include "core/folder.h"
#include "core/folder_fs.h"
#include "core/output.h"
#include "core/input_file.h"
#ifdef WITH_AO
#include "core/output_ao.h"
#endif
#include "core/output_null.h"
#ifdef WITH_MAD
#include "core/decode_mp3.h"
#endif
#ifdef WITH_ID3TAG
#include "core/info_mp3.h"
#endif
#ifdef WITH_VORBIS
#include "core/decode_ogg.h"
#include "core/info_ogg.h"
#endif
#ifdef WITH_FLAC
#include "core/decode_flac.h"
#include "core/info_flac.h"
#endif
#ifdef WITH_MIKMOD
#include "core/decode_module.h"
#include "core/info_module.h"
#endif
#ifdef WITH_SIDPLAY2
#include "core/decode_sid.h"
#include "core/info_sid.h"
#endif
#ifdef WITH_CURL
#include "core/input_remote.h"
#endif
#include "core/exceptions.h"

#define PAIR_STATUS		1
#define PAIR_BROWSER		2
#define PAIR_INFO		3

using namespace std;

WINDOW* winStatus;
WINDOW* winBrowser;
WINDOW* winInfo;

Folder*  folder;
Decoder* decoder = NULL;
Input*   input = NULL;
Output*  output = NULL;
Visualizer* visualizer = NULL;
Info*    info = NULL;

pthread_t	player_thread;
bool		havePlayerThread = false;
bool		playerPaused = false;

unsigned int browser_first_item = 0;
unsigned int browser_sel_item   = 0;
unsigned int browser_lines      = 0;

void*
player_wrapper(void* data)
{
	decoder->run();
	return NULL;
}

void
fillStatus()
{
	unsigned int playingTime = 0, totalTime = 0;

	werase(winStatus);
	box(winStatus, 0, 0);

	if (decoder != NULL) {
		playingTime = decoder->getPlayingTime();
		totalTime = decoder->getTotalTime();
	}

	const char* s;
	s = "Unknown Artist";
	if (info != NULL && info->getArtist() != NULL) s = info->getArtist();
	mvwprintw(winStatus, 1, 2, s);
	s = "Unknown Album";
	if (info != NULL && info->getAlbum() != NULL) s = info->getAlbum();
	mvwprintw(winStatus, 2, 2, s);
	s = "Unknown Title";
	if (info != NULL && info->getTitle() != NULL) s = info->getTitle();
	mvwprintw(winStatus, 3, 2, s);

	char temp[64];
	snprintf(temp, sizeof(temp), "%u:%02u / %u:%02u", playingTime / 60, playingTime % 60, totalTime / 60, totalTime % 60);
	mvwprintw(winStatus, 5, 2, temp);

	wrefresh(winStatus);

	alarm(1);
}

void
fillBrowser()
{
	int x;
	unsigned int line = 0;

	getmaxyx(winBrowser, browser_lines, x);

	/* ensure that whatever is selected fits on the screen */
	while (browser_sel_item < browser_first_item) {
		/* scroll the screen a bit */
		browser_first_item -= (browser_lines / 2);
	}
	while (browser_sel_item - browser_first_item >= browser_lines) {
		/* scroll the screen a bit */
		browser_first_item += (browser_lines / 2);
	}

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
init()
{
	/* Initialize curses */
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

	raw(); cbreak();
	keypad(stdscr, TRUE);
	noecho();
	refresh();
	curs_set(0);
	
	winStatus  = newwin(7, 0, 0, 0);
	winBrowser = newwin(LINES - 14, 0, 7, 0);
	winInfo    = newwin(7,  0, LINES - 7, 0);

	wattrset(winStatus, COLOR_PAIR(PAIR_STATUS) | A_BOLD);
	wbkgdset(winStatus, COLOR_PAIR(PAIR_STATUS));
	fillStatus();

	wattron(winBrowser, COLOR_PAIR(PAIR_BROWSER));
	wrefresh(winBrowser);

	wattron(winInfo, COLOR_PAIR(PAIR_INFO) | A_BOLD);
	wbkgdset(winInfo, COLOR_PAIR(PAIR_STATUS));
	werase(winInfo);
	box(winInfo, 0, 0);
	mvwprintw(winInfo, 1, 2, "up/dn/pgup/pgdn/home/end   browse                   space  pause/continue");
	mvwprintw(winInfo, 2, 2, "right/enter                select");
	mvwprintw(winInfo, 3, 2, "left/backspace             leave folder             f10    exit");
	wrefresh(winInfo);

	fillBrowser();

#if 0
	wprintw(winBrowser, "browser");
	wprintw(winInfo, "info");
#endif

	refresh();
}

void
stop()
{
	if (!havePlayerThread)
		return;

	/* Ask the decoder thread to terminate, and wait until it is gone */
	decoder->terminate();
	pthread_join(player_thread, NULL);

	havePlayerThread = false;
}

void
pauseplayer()
{
	if (playerPaused || !havePlayerThread)
		return;

	pthread_suspend_np(player_thread);
	playerPaused = true;
}

void
contplayer()
{
	if (!playerPaused || !havePlayerThread)
		return;

	pthread_resume_np(player_thread);
	playerPaused = false;
}

void
playFile()
{
	string currentFile = folder->getFullPath(folder->getEntries()[browser_sel_item]);

	contplayer();
	stop();

#ifdef WITH_CURL
	/*
	 * If we find :// in the filename and CURL is available, assume we
	 * are playing a stream.
	 */
	if (currentFile.find("://") != std::string::npos) {
		input = new InputRemote(currentFile);
	} else
#endif
		input = new InputFile(currentFile);

	string extension = string(currentFile.begin() + currentFile.find_last_of(".") + 1, currentFile.end());
#ifdef WITH_VORBIS
	if (!strcasecmp(extension.c_str(), "ogg")) {
		decoder = new DecoderOgg(input, output, visualizer);
		info = new InfoOgg(decoder);
	} else
#endif /* WITH_VORBIS */
#ifdef WITH_FLAC
	if (!strcasecmp(extension.c_str(), "flac")) {
		decoder = new DecoderFLAC(input, output, visualizer);
		info = new InfoFLAC(decoder);
	} else
#endif /* WITH_FLAC */
#ifdef WITH_MIKMOD
	if (!strcasecmp(extension.c_str(), "mod")) {
		decoder = new DecoderModule(input, output, visualizer);
		info = new InfoModule(decoder);
	} else
#endif /* WITH_MIKMOD */
#ifdef WITH_SIDPLAY2
	if (!strcasecmp(extension.c_str(), "sid")) {
		decoder = new DecoderSID(input, output, visualizer);
		info = new InfoSID(decoder);
	} else
#endif /* WITH_SIDPLAY2 */
#ifdef WITH_MAD
	{
		/* assume MP3 */
		decoder = new DecoderMP3(input, output, visualizer);
#ifdef WITH_ID3TAG
		info = new InfoMP3(decoder);
#endif
#else /* WITH_MAD */
	{
		delete input; input = NULL;
		return;
#endif /* !WITH_MAD */
	}
	if (info != NULL)
		info->load(currentFile.c_str());

	pthread_create(&player_thread, NULL, player_wrapper, NULL);
	havePlayerThread = true; playerPaused = false;
	fillStatus();
}

void
handleBrowser(int c)
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
			if (playerPaused)
				contplayer();
			else
				pauseplayer();
			break;
	}
}

void
cleanup()
{
	delwin(winStatus);
	delwin(winBrowser);
	delwin(winInfo);
	endwin();
}

void
handleUpdate(int n)
{
	fillStatus();
}

int
main(int argc, char** argv)
{

	folder = new FolderFS("/geluid");

	init();

	output = new OutputAO();

	signal(SIGALRM, handleUpdate);

	int c;	
	while ((c = getch()) != KEY_F(10)) {
		handleBrowser(c);
	}

	stop();
	cleanup();

	return EXIT_SUCCESS;
}
