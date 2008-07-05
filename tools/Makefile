CFLAGS=		-I/usr/local/include -I/usr/local/include/freetype2
LIBS=		-L/usr/local/lib -lfreetype

convert_font:	convert_font.cc
		$(CXX) -o convert_font convert_font.cc $(CFLAGS) $(LIBS)

clean:
		rm -f convert_font
