#include <stdio.h>
#include "exceptions.h"
#include "input_file.h"

using namespace std;

InputFile::InputFile(string resource) :
	Input(resource)
{
	f = fopen(resource.c_str(), "rb");
	if (f == NULL)
		throw InputException(string("Unable to open file ") + resource);

	fseek(f, 0, SEEK_END);length = ftell(f); rewind(f);
}

InputFile::~InputFile()
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

bool
InputFile::seek(size_t offset)
{
	fseek(f, offset, SEEK_SET);
	return true;
}
