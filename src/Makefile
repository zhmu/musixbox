OBJS=		main.o decode_mp3.o output_ao.o visualize_spectrum.o
CXXFLAGS=	-I/usr/local/include -I../include -g
LIBS=		-L/usr/local/lib -lmad -lao -lfftw3 -lSDL

musicd:		$(OBJS)
		$(CXX) -o musicd $(OBJS) $(LIBS)

clean:
		rm -f musicd $(OBJS)
