#ifndef __INTERACTION_H__
#define __INTERACTION_H__

//! \brief Abstract class to provide interaction between the player and the user
class Interaction {
public:
	Interaction() {
		terminating = 0;
	}

	/*! \brief Initialize interaction provider
	 *  \return Non-zero on success
	 */
	virtual int init() { return 1; }

	//! \brief Deinitialize interaction provider
	virtual void done() { }

	//! \brief Returns the height of the screen
	virtual int getHeight() = 0;

	//! \brief Returns the width of the screen
	virtual int getWidth() = 0;

	//! \brief Give the interaction environment a chance to update
	virtual void yield() { }

	//! \brief Check whether the application should terminate
	virtual int mustTerminate() { return terminating; }

	//! \brief Returns the size of a text string
	virtual int getTextHeight() = 0;

	/*! \brief Place a pixel on the interaction provider
	 *  \param x X-position
	 *  \param y Y-position
	 *  \param c Color to use
	 */
	virtual void putpixel(int x, int y, int c) = 0;

	/*! \brief Clears part of the interaction provider
	 *  \param x X-position
	 *  \param y Y-position
	 *  \param h Height to clear
	 *  \param w Width to clear
	 */
	virtual void clear(int x, int y, int h, int w) = 0;

	/*! \brief Place text on the interaction provider
	 *  \param x X-position
	 *  \param y Y-position
	 *  \param s Text to place
	 */
	virtual void puttext(int x, int y, const char* s) = 0;

	/*! \brief Returns text height and width
	 *  \param h Height of the text
	 *  \param w Width of the text
	 */
	virtual void gettextsize(const char* s, int* h, int* w) = 0;

	/* \brief Returns interaction coordinates, if any
	 * \param x X-coordinate
	 * \param y Y-coordinate
	 * \returns Non-zero if there was interaction
	 */
	virtual int getCoordinates(int* x, int* y) = 0;

protected:
	//! \brief Determines whether we should terminate
	int terminating;
};

#endif /* __INTERACTION_H__ */
