#ifndef HANDLER_XMPP_PLUGIN_H
#define HANDLER_XMPP_PLUGIN_H

#include <qutim/plugin.h>

class XmppHandlerPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_CLASSINFO("Uses", "UriHandler")

public:
	void init();
	bool load();
	bool unload();
};

#endif // HANDLER_XMPP_PLUGIN_H
