#ifndef __FONT__
#define __FONT__

struct CHARACTER {
	int height, width, yshift, advance_x;
	const char* data;
};

struct FONT {
	int height;
	struct CHARACTER* chars;
};

extern struct FONT font8;
extern struct FONT font12;

#define DEFAULT_FONT font12

#endif /* __FONT__ */
