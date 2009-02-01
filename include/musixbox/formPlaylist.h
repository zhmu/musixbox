#include <map>
#include <string>
#include "core/folder.h"
#include "ui/form.h"
#include "ui/image.h"
#include "ui/label.h"
#include "interface.h"

#ifndef __FORMPLAYLIST_H__
#define __FORMPLAYLIST_H__

class formPlaylist : public Form {
public:
	formPlaylist(Interaction* in, Interface* iface);

	int run();

protected:
	void update();
	void interact(Control* control);

private:
	std::vector<Label*> itemlabel;
	Image* bDown;
	Image* bUp;
	Image* bLeave;

	Interface* interface;

	unsigned int current_index;

	bool dirty, rehash;
};

#endif /* __FORMPLAYLIST_H__ */
