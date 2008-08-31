bin_PROGRAMS =	musixbox
AM_CXXFLAGS =	-Wall

INCLUDES =	../include
musixbox_SOURCES=	decode.cc decode_tone.cc \
		font.cc info.cc input_file.cc \
		interaction.cc interaction_chain.cc \
		interface.cc main.cc \
		interaction_avr.cc \
		formBrowser.cc formPlayer.cc \
		mixer_oss.cc
musixbox_LDADD=	../lib/ui/libui.a

if MAD
musixbox_SOURCES+=	decode_mp3.cc
endif
if ID3TAG
musixbox_SOURCES+=	info_mp3.cc
endif
if VORBIS
musixbox_SOURCES+=	decode_ogg.cc info_ogg.cc
endif
if AO
musixbox_SOURCES+=	output_ao.cc
endif
if FLAC
musixbox_SOURCES+=	decode_flac.cc info_flac.cc
endif
if SDL
musixbox_SOURCES+=	interaction_sdl.cc
endif
if FFTW
musixbox_SOURCES+=	visualize_spectrum.cc
endif
