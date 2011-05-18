#ifndef MPLUGIN_H
#define MPLUGIN_H

#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

class DPlugin : public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "Freedesktop")
public:
			DPlugin();
	void init();
	bool load();
	bool unload();
};

#endif // MPLUGIN_H
