#include <curl/curl.h>
#include <pthread.h>
#include <stdio.h>
#include <string>
#include "exceptions.h"
#include "input_remote.h"

#define MIN(a,b) ((a)<(b) ? (a) : (b))

size_t
remote_process_data(void* buffer, size_t size, size_t nmemb, void* userp)
{
	InputRemote* input = (InputRemote*)userp;

	/* Ensure it is possible to store the data we get offered */
	size_t totalsz = MIN(size * nmemb, INPUT_REMOTE_CACHESIZE);

	/* Ensure we have space to store this data */
	pthread_mutex_lock(&input->mtx_data);
	while (input->bytes_avail + totalsz >= INPUT_REMOTE_CACHESIZE) {
		/* Wait until data has been read */
		pthread_cond_wait(&input->cv_data_read, &input->mtx_data);
			
	}

	/* Process all data that has been handed to us */
	char* ptr = (char*)buffer;
	while (totalsz > 0) {
		/* Preform a case distinction */
		if (input->read_pos > input->write_pos) {
			/*
			 * The buffer looks like this:
			 *
			 * 0                                        n
			 * +-----------+-------------+--------------+
			 * |%%%%%%%%%%%|             |%%%%%%%%%%%%%%|
			 * +-----------+-------------+--------------+
			 *             ^             ^
			 *          write_pos      read_pos
			 *
			 * This means we have at most read_pos - write_pos to write.
			 *
			 */
			size_t chunklen = MIN(input->read_pos - input->write_pos, totalsz);
			memcpy((input->cache + input->write_pos), ptr, chunklen);
			input->write_pos = (input->write_pos + chunklen) % INPUT_REMOTE_CACHESIZE;
			totalsz -= chunklen; ptr += chunklen;
			input->bytes_avail += chunklen;
		} else {
			/*
			 * The buffer looks like this:
			 *
			 *     0                                        n
			 *     +-----------+-------------+--------------+
			 *  +->|           |%%%%%%%%%%%%%|              | << chunk1
			 *  |  +-----------+-------------+--------------+
			 *  |              ^             ^
			 *chunk2        read_pos       write_poo
			 *
			 * This means we have at most n - write_pos + read_pos to write.
			 *                            \---chunk1--/   \chunk2/
			 *
			 */
			size_t chunklen = MIN(INPUT_REMOTE_CACHESIZE - input->write_pos, totalsz);
			memcpy((input->cache + input->write_pos), ptr, chunklen);
			input->write_pos = (input->write_pos + chunklen) % INPUT_REMOTE_CACHESIZE;
			totalsz -= chunklen; ptr += chunklen;
			input->bytes_avail += chunklen;

			/* handle chunk2 */
			chunklen = MIN(input->read_pos, totalsz);
			memcpy((input->cache + input->write_pos), ptr, chunklen);
			input->write_pos = (input->write_pos + chunklen) % INPUT_REMOTE_CACHESIZE;
			totalsz -= chunklen; ptr += chunklen;
			input->bytes_avail += chunklen;
		}
	}

	/* Wake up the reader */
	pthread_cond_signal(&input->cv_data_available);
	pthread_mutex_unlock(&input->mtx_data);

	/* If we need to terminate, do it */
	if (input->terminating)
		return CURLE_WRITE_ERROR;
	return MIN(size * nmemb, INPUT_REMOTE_CACHESIZE);
}

void*
remote_fetcher(void* ptr)
{
	InputRemote* input = (InputRemote*)ptr;

	CURLcode i = curl_easy_perform(input->curl);
	
	/*
	 * We may have succeeded, we might have not; mark us as completed in
	 * any case.
	 */
	pthread_mutex_lock(&input->mtx_data);
	input->completed = true;
	pthread_mutex_unlock(&input->mtx_data);

	pthread_exit(NULL);
}

InputRemote::InputRemote(std::string resource) :
	Input(resource)
{
	if (curl_global_init(CURL_GLOBAL_ALL))
		throw InputException(std::string("InputRemote: unable to initialize curl library"));
	curl = curl_easy_init();
	if (curl == NULL)
		throw InputException(std::string("InputRemote: unable to create curl object"));
	if ((cache = (char*)malloc(INPUT_REMOTE_CACHESIZE)) == NULL)
		throw InputException(std::string("InputRemote: out of memory"));

	/* We are at the start of the buffer, and have not completed */
	read_pos = 0; write_pos = 0; bytes_avail = 0; completed = false; terminating = false;

	/* Create synchronization primitives */
	pthread_mutex_init(&mtx_data, NULL);
	pthread_cond_init(&cv_data_available, NULL);
	pthread_cond_init(&cv_data_read, NULL);

	/* Start the CURL transfer */
	curl_easy_setopt(curl, CURLOPT_URL, resource.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, remote_process_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
	if (pthread_create(&thr_fetcher, NULL, remote_fetcher, this))
		throw InputException(std::string("InputRemote: unable to create thread"));
}

InputRemote::~InputRemote()
{
	/* Signal the thread that we want to get rid of it */
	terminating = true;
	
	/* Wait until the thread is gone */
	pthread_join(thr_fetcher, NULL);

	if (curl != NULL)
		curl_easy_cleanup(curl);
	curl_global_cleanup();

	pthread_cond_destroy(&cv_data_available);
	pthread_cond_destroy(&cv_data_read);
	pthread_mutex_destroy(&mtx_data);
	free(cache);
}

size_t
InputRemote::read(char* buffer, size_t len)
{
	pthread_mutex_lock(&mtx_data);
	if (bytes_avail == 0) {
		if (completed) {
			/* All available data has been passed - end of stream */
			pthread_mutex_unlock(&mtx_data);
			return 0;
		}

		/* Wait until data is available */
		pthread_cond_wait(&cv_data_available, &mtx_data);
	}

	/* Attempt to read up to len bytes */
	size_t total = 0;
	while (len > 0) {
		if (read_pos >= write_pos) {
			/*
			 * The buffer looks like this:
			 *
			 *    0                                        n
			 *    +-----------+-------------+--------------+
			 *  >-|%%%%%%%%%%%|             |%%%%%%%%%%%%%%| << chunk1
			 *  | +-----------+-------------+--------------+
			 *  |             ^             ^    
			 *chunk2      write_pos      read_pos
			 *
			 * This means we have n - read_pos + write_pos * bytes available.
			 *                    \--chunk1--/   \chunk2-/
			 *
			 */

			/* First transfer from here till the end of the cache */
			size_t chunklen = MIN(INPUT_REMOTE_CACHESIZE - read_pos, len);
			memcpy((buffer + total), (cache + read_pos), chunklen);
			read_pos = (read_pos + chunklen) % INPUT_REMOTE_CACHESIZE;
			len -= chunklen; total += chunklen; bytes_avail -= chunklen;

			/* Handle chunk2 */
			chunklen = MIN(write_pos, len);
			memcpy((buffer + total), (cache + read_pos), chunklen);
			read_pos = (read_pos + chunklen) % INPUT_REMOTE_CACHESIZE;
			len -= chunklen; total += chunklen; bytes_avail -= chunklen;
		} else {
			/*
			 * The buffer looks like this:
			 *
			 * 0                                        n
			 * +-----------+-------------+--------------+
			 * |           |%%%%%%%%%%%%%|              |
			 * +-----------+-------------+--------------+
			 *             ^             ^
			 *          read_pos       write_poo
			 *
			 * This means we have at most write_pos - read_pos to read.
			 *
			 */
			size_t chunklen = MIN(write_pos - read_pos, len);
			memcpy((buffer + total), (cache + read_pos), chunklen);
			read_pos = (read_pos + chunklen) % INPUT_REMOTE_CACHESIZE;
			len -= chunklen; total += chunklen; bytes_avail -= chunklen;
		}

		/*
		 * If there is no more data available, stop. Note that we cannot
		 * use this condition previously, because if initially exactly
		 * INPUT_REMOTE_CACHESIZE bytes were available, read_pos == write_pos
		 * and we'd return with 0 bytes read...
		 */
		if (read_pos == write_pos)
			break;
	}

	/* If the writer is blocked because the input cache was full, wake it */
	pthread_cond_signal(&cv_data_read);

	pthread_mutex_unlock(&mtx_data);
	return total;
}
