#include <sys/time.h>
#include "font.h"

#ifndef __INTERACTION_H__
#define __INTERACTION_H__

#define INTERACTION_TYPE_NONE		0
#define INTERACTION_TYPE_NORMAL		1
#define INTERACTION_TYPE_ALTERNATIVE	2

//! \brief Abstract class to provide interaction between the player and the user
class Interaction {
public:
	Interaction() {
		mustBeTerminating = false; intX = 0; intY = 0;
		intType = INTERACTION_TYPE_NONE;
		intTimestamp.tv_sec = 0; intTimestamp.tv_usec = 0;
	}

	virtual ~Interaction() { }

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
	unsigned int getTextHeight();

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
	 *  \param font Font to use
	 */
	virtual void puttext(unsigned int x, unsigned int y, const char* s, FONT* font);

	/*! \brief Returns text height and width
	 *  \param h Height of the text
	 *  \param w Width of the text
	 *  \param font Font to use
	 */
	virtual void gettextsize(const char* s, unsigned int* h, unsigned int* w, FONT* font);

	/* \brief Returns interaction, if any
	 * \param x X-coordinate
	 * \param y Y-coordinate
	 * \param type Interaction type
	 * \returns True if there was interaction
	 */
	virtual bool getInteraction(unsigned int* x, unsigned int* y, unsigned int* type);

	//! \brief Flushes any unreported interaction 
	virtual void flushInteraction();
	
protected:
	/* \brief Update interaction coordinates
	 * \param x X-coordinate
	 * \param y Y-coordinate
	 * \param type Interaction type
	 */
	virtual void setInteraction(unsigned int x, unsigned int y, unsigned int type);

	//! \brief Determines whether we should terminate
	bool mustBeTerminating;

	/*! \brief Calculates the amount of milliseconds between now and the last interaction */
	unsigned long calculateInteractionDelta();

private:
	//! \brief Interaction: X and Y coordinates and type
	unsigned int intX, intY, intType;

	//! \brief Timestamp of last interaction
	struct timeval intTimestamp;
};

#endif /* __INTERACTION_H__ */
