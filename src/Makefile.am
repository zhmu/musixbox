bin_PROGRAMS =	musicd
AM_CXXFLAGS =	-Wall

INCLUDES =	../include
musicd_SOURCES=	decode.cc decode_tone.cc \
		font.cc info.cc input_file.cc \
		interaction.cc interaction_chain.cc \
		interface.cc main.cc \
		interaction_avr.cc \
		formBrowser.cc formPlayer.cc
musicd_LDADD=	../lib/ui/libui.a

if MAD
musicd_SOURCES+=	decode_mp3.cc
endif
if ID3TAG
musicd_SOURCES+=	info_mp3.cc
endif
if VORBIS
musicd_SOURCES+=	decode_ogg.cc info_ogg.cc
endif
if AO
musicd_SOURCES+=	output_ao.cc
endif
if FLAC
musicd_SOURCES+=	decode_flac.cc info_flac.cc
endif
if SDL
musicd_SOURCES+=	interaction_sdl.cc
endif
if FFTW
musicd_SOURCES+=	visualize_spectrum.cc
endif
