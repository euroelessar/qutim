#ifndef URIHANDLER_H
#define URIHANDLER_H

#include <qutim/plugin.h>

class UriHandlerPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
public:
	void init();
	bool load();
	bool unload();
};

#endif // URIHANDLER_H
