OBJS=		main.o decode_mp3.o output_ao.o visualize_spectrum.o decode.o \
		decode_tone.o input_file.o decode_ogg.o interaction_sdl.o \
		interface.o info.o info_mp3.o info_ogg.o decode_flac.o \
		info_flac.o font.o interaction_chain.o
CXXFLAGS=	-I/usr/local/include -I../include -g
LIBS=		-L/usr/local/lib -lmad -lao -lfftw3 -lSDL -logg -lvorbis -lvorbisfile -lpthread \
		-lid3tag -lFLAC++
 
musicd:		$(OBJS)
		$(CXX) -o musicd $(OBJS) $(LIBS)

clean:
		rm -f musicd *core* font.cc $(OBJS)
		cd ../tools && $(MAKE) clean

font.cc:	../tools/convert_font
		../tools/convert_font > font.cc

../tools/convert_font:
		cd ../tools && $(MAKE) convert_font
