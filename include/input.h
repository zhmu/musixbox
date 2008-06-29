#ifndef __INPUT_H__
#define __INPUT_H__

//! \brief Abstract class to provide stream input
class Input {
public:
	/*! \brief Read data from the stream
	 *  \param buffer Output buffer to place data in
	 *  \param len Number of bytes to read
	 *  \return Number of bytes read, 0 if end of stream
	 */
	virtual size_t read(char* buffer, size_t len) = 0;
};

#endif /* __INPUT_H__ */
