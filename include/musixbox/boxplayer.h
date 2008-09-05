#include "core/player.h"

#ifndef __BOXPLAYER_H__
#define __BOXPLAYER_H__

class Interface;

class BoxPlayer : public Player {
public:
	/*! \brief Construct a box player object
	 *  \param resource Resource to use
	 *  \param o Output object to use
	 *  \param v Visualiser object to use
	 */
	BoxPlayer(std::string resource, Output* o, Visualizer* v, Interface* i);

	//! \brief Called if the player successfully terminated
	virtual void terminated();

protected:
	//! \brief Interface object
	Interface* interface;
};

#endif /* __BOXPLAYER_H__ */
