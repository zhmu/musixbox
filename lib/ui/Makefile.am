INCLUDES =		../../include
noinst_LIBRARIES=	libui.a
libui_a_SOURCES=	label.cc control.cc form.cc image.cc font.cc  \
			interaction.cc interaction_chain.cc \
			interaction_avr.cc
if SDL
libui_a_SOURCES+=	interaction_sdl.cc
endif
