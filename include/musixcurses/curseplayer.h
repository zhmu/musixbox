#include "core/player.h"

#ifndef __CURSEPLAYER_H__
#define __CURSEPLAYER_H__

class Interface;

class CursePlayer : public Player {
public:
	/*! \brief Construct a box player object
	 *  \param resource Resource to use
	 *  \param o Output object to use
	 *  \param i Corresponding interface object
	 */
	CursePlayer(std::string resource, Output* o, Interface* i);

	//! \brief Called if the player successfully terminated
	virtual void terminated();

protected:
	//! \brief Interface object
	Interface* interface;
};

#endif /* __CURSEPLAYER_H__ */
