#include "handler_xmpp_plugin.h"
#include "handler_xmpp.h"
#include "../../src/urihandlerservice.h"

void XmppHandlerPlugin::init()
{

}

bool XmppHandlerPlugin::load()
{
	return UriHandlerService::addHandler<XmppHandler>("xmpp");
}

bool XmppHandlerPlugin::unload()
{
	return UriHandlerService::removeHandler("xmpp");
}

QUTIM_EXPORT_PLUGIN(XmppHandlerPlugin)
