#include <curl/curl.h>
#include <pthread.h>
#include "input.h"

#ifndef __INPUT_REMOTE_H_
#define __INPUT_REMOTE_H__

#define INPUT_REMOTE_CACHESIZE		(64*1024)

//! \brief Handles streaming input from a remote location, using libcurl
class InputRemote : public Input {
friend	size_t remote_process_data(void* buffer, size_t size, size_t nmemb, void* userp);
friend	void*  remote_fetcher(void* ptr);
public:
	/*! \brief Initializes the remote streaming object
	 *  \param resource URL to stream
	 */
	InputRemote(std::string resource);

	//! \brief Destructs the remote streaming object
	~InputRemote();

	/*! \brief Reads a chunk from the file
	 *  \param buffer Output buffer
	 *  \param len Number of bytes to read
	 *  \return Number of bytes read
	 */
	size_t read(char* buffer, size_t len);
	
	/*! \brief Seek to an offset within the input stream
	 *  \param offset Offset to seek to, in bytes
	 *  \return true on success, false is the seek failed
	 */
	inline bool seek(size_t offset) { return false; }

	//! \brief Returns the length of the file, in bytes
	inline size_t getLength() { return 0; }

	//! \brief Returns current file position, in bytes
	inline size_t getCurrentPosition() { return 0; }

protected:
	//! \brief Mutex used to protect the data buffer
	pthread_mutex_t mtx_data;

	//! \brief Condition variable used to signal that data is available
	pthread_cond_t cv_data_available;

	//! \brief Condition variable used to signal that data was read
	pthread_cond_t cv_data_read;

	//! \brief Fetcher thread, responsible for fetching data and signalling availability
	pthread_t thr_fetcher;
	
	//! \brief Curl handle
	CURL* curl;

	//! \brief Input cache
	char* cache;

	//! \brief Read and write pointers
	size_t read_pos, write_pos;

	//! \brief Amount of data available
	size_t bytes_avail;

	//! \brief Are we done yet?
	bool completed;

	//! \brief Do we have to terminate
	bool terminating;
};

#endif /* __INPUT_REMOTE_H__ */
