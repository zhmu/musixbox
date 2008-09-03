#include <string>

#ifndef __INPUT_H__
#define __INPUT_H__

//! \brief Abstract class to provide stream input
class Input {
public:
	/*! \brief Constructs the input stream object
	 *  \param resource Resource to open
	 */
	inline Input(std::string resource) { }

	//! \brief Destructs the input stream object
	inline virtual ~Input() { }

	/*! \brief Read data from the stream
	 *  \param buffer Output buffer to place data in
	 *  \param len Number of bytes to read
	 *  \return Number of bytes read, 0 if end of stream
	 */
	virtual size_t read(char* buffer, size_t len) = 0;

	/*! \brief Retrieve size of input stream
	 *  \return Number of bytes, or 0 if this is unknown
	 */
	virtual size_t getLength() { return 0; }

	/*! \brief Seek to an offset within the input stream
	 *  \param offset Offset to seek to, in bytes
	 *  \return true on success, false is the seek failed
	 *
	 *  offset is always relative to the beginning of the file.
	 */
	virtual bool seek(size_t offset) { return false; }

	/*! \brief Retrieve current position in input stream
	 *  \return Current byte position, or 0 if this is unknown
	 */
	virtual size_t getCurrentPosition() { return 0; }
};

#endif /* __INPUT_H__ */
