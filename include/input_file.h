#include <stdio.h>
#include "input.h"

#ifndef __INPUT_FILE__H_
#define __INPUT_FILE_H__

//! \brief Handles streaming input from a file
class InputFile : public Input {
public:
	/*! \brief Opens a file on disk
	 *  \param fname File name to open
	 *  \return Non-zero on success
	 */
	int open(const char* fname);

	//! \brief Closes a previously-opened file
	void close();

	/*! \brief Reads a chunk from the file
	 *  \param buffer Output buffer
	 *  \param len Number of bytes to read
	 *  \return Number of bytes read
	 */
	size_t read(char* buffer, size_t len);

	//! \brief Returns the length of the file, in bytes
	size_t getLength();

	//! \brief Returns current file position, in bytes
	size_t getCurrentPosition();

private:
	//! \brief File handle
	FILE* f;

	//! \brief File length
	size_t length;
};

#endif /* __INPUT_FILE_H__ */
