OBJS=		main.o decode_mp3.o output_ao.o visualize_spectrum.o decode.o \
		decode_tone.o input_file.o decode_ogg.o interaction_sdl.o \
		interface.o
CXXFLAGS=	-I/usr/local/include -I../include -g
LIBS=		-L/usr/local/lib -lmad -lao -lfftw3 -lSDL -logg -lvorbis -lvorbisfile -lSDL_ttf

musicd:		$(OBJS)
		$(CXX) -o musicd $(OBJS) $(LIBS)

clean:
		rm -f musicd *core* $(OBJS)
