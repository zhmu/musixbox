#ifndef __VISUALIZE_H__
#define __VISUALIZE_H__

/*! \brief Abstract class to provide a visualizer
 */
class Visualizer {
public:
	/*! \brief Initializes the visualizer
	 *  \return Non-zero on success
	 */
	virtual int init() { return 1; };

	/*! \brief Update visualizer content
	 *  \param audio Raw audio output in 16 bit PCM format
	 *  \param num Number of samples in the output stream
	 *
	 *  The size of the output stream must be at least 4 * num, as
	 *  every output consists of 2x 16 bit values (Left, Right)
	 */
	virtual void update(const char* audio, unsigned int num) = 0;

	//! \brief Deinitialize the visualizer
	virtual void done() { };
};

#endif /* __VISUALIZE_H__ */
