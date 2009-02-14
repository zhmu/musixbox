#include <sys/types.h>
#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H

unsigned int font_size;

FT_Library ftlib;
FT_Face face;

void
loadfont(const char* fname)
{
	FILE* f;
	unsigned long len;

	if (FT_New_Face(ftlib, fname, 0, &face))
		errx(1, "unable to initialize load font");

	if (!FT_IS_SCALABLE(face))
		errx(1, "font is not scalable");

	if (FT_Set_Char_Size(face, 0, font_size * 64, 0, 0))
		errx(1, "can't request font size");
}

void
usage()
{
	fprintf(stderr, "usage: conver_font font.ttf size\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char* argv[])
{
	char* ptr;
	if (argc < 2)
		usage();

	font_size = strtoul(argv[2], &ptr, 10);
	if (font_size == 0 || *ptr != '\0') {
		fprintf(stderr, "unable to parse number '%s'\n", argv[2]);
		usage();
	}

	FT_Error error = FT_Init_FreeType(&ftlib);
	if (error)
		errx(1, "unable to initialize freetype library");

	//loadfont("/usr/local/lib/X11/fonts/bitstream-vera/Vera.ttf");
	//loadfont("/usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf");
	loadfont(argv[1]);

	printf("/* This file was automatically generated - do not edit! */\n");
	printf("#include \"ui/font.h\"\n");
	printf("\n");
	printf("static struct CHARACTER fontData%u[] = {\n", font_size);
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
		 * XXX: limiting ourselves to chars of maximally 8 pixels wide
		 * simplies the code considerably, so do that for now...
		 */
		if (slot->bitmap.width > 8)
			abort();

		/* C will parse a \ as: continue on the next line, so avoid it :-/ */
		printf((isprint(ch) && (ch != '\\')) ? "\t// %c\n" : "\t// %u\n", ch);

		if (ch != 0x20) {
			printf("\t{ %u, %u, %i, %u, \"", slot->bitmap.rows, slot->bitmap.width, slot->bitmap_top, slot->advance.x >> 6);
			for (int j = 0; j < slot->bitmap.rows; j++) {
				for (int i = 0; i <= 0 /* XXX: this works due to the 8-width-limit */; i++) {
					unsigned char* ptr = slot->bitmap.buffer +
						(j * slot->bitmap.pitch + i);

					/*
					 * Once rendered by FreeType, the bits
					 * are ordered exactly the opposite of
					 * how we would like them, so flip 'm.
					 */
					uint8_t ch = 0;
					for (int b = 0; b < 8; b++) {
						if (*ptr & (1 << (8 - b)))
							ch |= (1 << b);
					}
					printf("\\x%02x", ch);
				}
			}
			printf("\" },\n");
		} else {
			/* Most font files do not provide a space, so roll our own! */
			printf("\t{ %u, %u, 0, %u, \"", font_size, font_size / 2, (slot->advance.x >> 6) / 2);
			for (int i = 0; i < font_size; i++)
				printf ("\\x00");
			printf("\" },\n");
		}
	}
	printf("};\n");
	printf("\n");
	printf("struct FONT font%u = { %u, fontData%u };\n",
	 	font_size, font_size, font_size);

	FT_Done_Face(face);
	FT_Done_FreeType(ftlib);

	return 0;
}
