#ifndef __INTERACTION_H__
#define __INTERACTION_H__

//! \brief Abstract class to provide interaction between the player and the user
class Interaction {
public:
	Interaction() {
		mustBeTerminating = false; coordX = 0; coordY = 0;
		haveValidCoords = false;
	}

	//! \brief Returns the height of the screen
	virtual unsigned int getHeight() = 0;

	//! \brief Returns the width of the screen
	virtual unsigned int getWidth() = 0;

	//! \brief Give the interaction environment a chance to update
	virtual void yield() { }

	//! \brief Check whether the application should terminate
	virtual bool mustTerminate() { return mustBeTerminating; }

	//! \brief Request termination
	virtual void requestTermination() { mustBeTerminating = true; }

	//! \brief Returns the size of a text string
	virtual inline unsigned int getTextHeight() { return 8; }

	/*! \brief Place a pixel on the interaction provider
	 *  \param x X-position
	 *  \param y Y-position
	 *  \param c Color to use
	 */
	virtual void putpixel(unsigned int x, unsigned int y, unsigned int c) = 0;

	/*! \brief Clears part of the interaction provider
	 *  \param x X-position
	 *  \param y Y-position
	 *  \param h Height to clear
	 *  \param w Width to clear
	 */
	virtual void clear(unsigned int x, unsigned int y, unsigned int h, unsigned int w);

	/*! \brief Place text on the interaction provider
	 *  \param x X-position
	 *  \param y Y-position
	 *  \param s Text to place
	 */
	virtual void puttext(unsigned int x, unsigned int y, const char* s);

	/*! \brief Returns text height and width
	 *  \param h Height of the text
	 *  \param w Width of the text
	 */
	virtual void gettextsize(const char* s, unsigned int* h, unsigned int* w);

	/* \brief Returns interaction coordinates, if any
	 * \param x X-coordinate
	 * \param y Y-coordinate
	 * \returns True if there was interaction
	 */
	virtual bool getCoordinates(unsigned int* x, unsigned int* y);

	//! \brief Is scrolling output desirable
	virtual bool isScrollingOK() { return true; }
	
protected:
	/* \brief Update interaction coordinates
	 * \param x X-coordinate
	 * \param y Y-coordinate
	 */
	virtual void setCoordinates(unsigned int x, unsigned int y);

	//! \brief Determines whether we should terminate
	bool mustBeTerminating;

private:
	//! \brief Do we have valid X/Y coordinates?
	bool haveValidCoords;

	//! \brief X and Y coordinates
	unsigned int coordX, coordY;
};

#endif /* __INTERACTION_H__ */