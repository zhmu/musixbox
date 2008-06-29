OBJS=		main.o decode_mp3.o output_ao.o visualize_spectrum.o decode.o \
		decode_tone.o input_file.o decode_ogg.o interaction_sdl.o \
		interface.o info.o info_mp3.o info_ogg.o
CXXFLAGS=	-I/usr/local/include -I../include -g
LIBS=		-L/usr/local/lib -lmad -lao -lfftw3 -lSDL -logg -lvorbis -lvorbisfile -lSDL_ttf -lpthread \
		-lid3tag

musicd:		$(OBJS)
		$(CXX) -o musicd $(OBJS) $(LIBS)

clean:
		rm -f musicd *core* $(OBJS)
