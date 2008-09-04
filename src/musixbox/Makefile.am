bin_PROGRAMS=		musixbox
AM_CXXFLAGS=		-Wall -I../../include

musixbox_LDADD=        ../../lib/core/libcore.a ../../lib/ui/libui.a
musixbox_SOURCES=	font.cc \
			interaction.cc interaction_chain.cc \
			interface.cc main.cc \
			interaction_avr.cc \
			formBrowser.cc formPlayer.cc

if SDL
musixbox_SOURCES+=	interaction_sdl.cc
endif
