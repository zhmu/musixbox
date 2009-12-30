AM_CPPFLAGS=		-I../../include/core
noinst_LIBRARIES=	libcore.a
libcore_a_SOURCES=	decode.cc decode_tone.cc \
			info.cc input_file.cc \
			mixer_oss.cc output_swmix.cc \
			folder.cc folder_fs.cc \
			decoderfactory.cc player.cc \
			outputmixerfactory.cc \
			folderfactory.cc \
			output_oss.cc

if MAD
libcore_a_SOURCES+=	decode_mp3.cc
endif
if MPG123
libcore_a_SOURCES+=	decode_mp3_mpg123.cc
endif
if ID3TAG
libcore_a_SOURCES+=	info_mp3.cc
endif
if ID3LIB
libcore_a_SOURCES+=	info_mp3_id3lib.cc
endif
if VORBIS
libcore_a_SOURCES+=	decode_ogg.cc info_ogg.cc
endif
if AO
libcore_a_SOURCES+=	output_ao.cc
endif
if FLAC
libcore_a_SOURCES+=	decode_flac.cc info_flac.cc
endif
if FFTW
libcore_a_SOURCES+=	visualize_spectrum.cc
endif
if MIKMOD
libcore_a_SOURCES+=	decode_module.cc info_module.cc
endif
if SIDPLAY2
libcore_a_SOURCES+=	decode_sid.cc info_sid.cc
endif
if CURL
libcore_a_SOURCES+=	input_remote.cc
if XML
libcore_a_SOURCES+=	folder_remote.cc
endif
endif
if ALSA
libcore_a_SOURCES+=	output_alsa.cc
endif
