#include <sys/types.h>
#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define PT_SIZE 8

FT_Library ftlib;
FT_Face face;

void
loadfont(const char* fname)
{
	FILE* f;
	unsigned long len;
	char* buf;

	if (FT_New_Face(ftlib, fname, 0, &face))
		errx(1, "unable to initialize load font");

	if (!FT_IS_SCALABLE(face))
		errx(1, "font is not scalable");

	if (FT_Set_Char_Size(face, 0, PT_SIZE * 64, 0, 0))
		errx(1, "can't request font size");
}

int
main(int argc, char* argv[])
{
	FT_Error error = FT_Init_FreeType(&ftlib);
	if (error)
		errx(1, "unable to initialize freetype library");

	loadfont("/usr/local/lib/X11/fonts/bitstream-vera/Vera.ttf");

	printf("/* This file was automatically generated - do not edit! */\n");
	printf("#include \"font.h\"\n");
	printf("\n");
	printf("struct CHARACTER theFont[] = {\n");
	for (FT_ULong ch = 0; ch <= 255; ch++ ) {
		int charidx = FT_Get_Char_Index(face, ch);
		if (charidx == 0) {
			printf("\t// %u\n", ch);
			printf("\t{ 0, 0, 0, 0, \"\" },\n");
			continue;
		}
		if (FT_Load_Glyph(face, charidx, FT_LOAD_MONOCHROME))
			err(1, "unable to render char '%c', skipping\n", ch);
		if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO))
			errx(1,"unable to render char '%c', skipping\n", ch);
		FT_GlyphSlot slot = face->glyph;

		/*
		 * We construct an in-memory rendering of the charachter and
		 * scrape all bits together on a per-column basis.
		 */
		char* buf = (char*)malloc(slot->bitmap.rows * slot->bitmap.width);
		if (buf == NULL)
			err(1, "can't allocate memory");

		/* Fill a buffer with the content */
		for (int j = 0 ; j < slot->bitmap.rows; j++) {
			for (int i = 0; i <= slot->bitmap.width / 8; i++) {
				unsigned char* ptr = slot->bitmap.buffer +
					(j * slot->bitmap.pitch + i);
				for (int v = 0; v < 8; v++) {
					buf[j * slot->bitmap.width + (i * 8) + v] = (*ptr & (1 << (7 - v))) ? 1 : 0;
				}
			}
		}
		/* C will parse a \ as: continue on the next line, so avoid it :-/ */
		printf((isprint(ch) && (ch != '\\')) ? "\t// %c\n" : "\t// %u\n", ch);

		if (ch != 0x20) {
			printf("\t{ %u, %u, %i, %u, \"", slot->bitmap.rows, slot->bitmap.width, slot->bitmap_top, slot->advance.x >> 6);
			for (int i = 0; i < slot->bitmap.width; i++) {
				uint8_t ch = 0;
				for (int j = 0 ; j < slot->bitmap.rows; j++) {
					ch >>= 1;
					if (buf[j * slot->bitmap.width + i])
						ch |= 0x80;
						if (j > 8 && j % 8 == 0) printf("\\x%02x", ch);
				}
				printf("\\x%02x", ch);
			}
			printf("\" },\n");
		} else {
			/* Most font files do not provide a space, so roll our own! */
			printf("\t{ %u, %u, 0, %u, \"", PT_SIZE, PT_SIZE / 2, slot->advance.x >> 6);
			for (int i = 0; i < PT_SIZE / 2; i++)
				printf ("\\x00");
			printf("\" },\n");
		}

		free(buf);
	}
	printf("};\n");


	FT_Done_Face(face);
	FT_Done_FreeType(ftlib);

	return 0;
}
