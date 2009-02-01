CFLAGS=		-I/usr/local/include -I/usr/local/include/freetype2
LIBS=		-L/usr/local/lib -lfreetype

all:		convert_font touch_test

convert_font:	convert_font.cc
		$(CXX) -o convert_font convert_font.cc $(CFLAGS) $(LIBS)

touch_test:	touch_test.cc
		$(CXX) -o touch_test touch_test.cc $(CFLAGS) $(LIBS)

clean:
		rm -f convert_font touch_test
