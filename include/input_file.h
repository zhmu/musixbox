#include <stdio.h>
#include "input.h"

#ifndef __INPUT_FILE__H_
#define __INPUT_FILE_H__

class InputFile : public Input {
public:
	int open(const char* fname);
	void close();

	size_t read(char* buffer, size_t len);

private:
	FILE* f;
};

#endif /* __INPUT_FILE_H__ */
