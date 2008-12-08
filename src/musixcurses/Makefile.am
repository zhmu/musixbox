bin_PROGRAMS=		musixcurses
AM_CXXFLAGS=		-Wall -I../../include -I../../include/musixcurses

musixcurses_LDADD=	../../lib/core/libcore.a ../../lib/misc/libmisc.a -lcurses
musixcurses_SOURCES=	main.cc interface.cc curseplayer.cc menu.cc menubrowser.cc menuplaylist.cc \
			lyrics.cc
