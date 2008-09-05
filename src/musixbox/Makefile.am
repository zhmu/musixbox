bin_PROGRAMS=		musixbox
AM_CXXFLAGS=		-Wall -I../../include -I../../include/musixbox

musixbox_LDADD=        ../../lib/core/libcore.a ../../lib/ui/libui.a
musixbox_SOURCES=	interaction.cc interaction_chain.cc \
			interface.cc main.cc boxplayer.cc \
			interaction_avr.cc \
			formBrowser.cc formPlayer.cc 

if SDL
musixbox_SOURCES+=	interaction_sdl.cc
endif
