#ifndef OSCARPLUGIN_H
#define OSCARPLUGIN_H

#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

class OscarPlugin : public Plugin
{
	Q_OBJECT
public:
    OscarPlugin();
	void init();
	bool load();
	bool unload();
};

#endif // OSCARPLUGIN_H
