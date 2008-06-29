#include <stdio.h>
#include "input_file.h"

int
InputFile::open(const char* fname)
{
	f = fopen(fname, "rb");
	return (f == NULL) ? 0 : 1;
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
