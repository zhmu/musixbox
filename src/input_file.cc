#include <stdio.h>
#include "input_file.h"

int
InputFile::open(const char* fname)
{
	f = fopen(fname, "rb");
	if (f == NULL)
		return 0;

	fseek(f, 0, SEEK_END);length = ftell(f); rewind(f);
	return 1;
}

void
InputFile::close()
{
	fclose(f);
}

size_t
InputFile::read(char* buffer, size_t len)
{
	return (fread(buffer, 1, len, f));
}

size_t
InputFile::getLength() {
	return length;
}

size_t
InputFile::getCurrentPosition()
{
	return ftell(f);
}
