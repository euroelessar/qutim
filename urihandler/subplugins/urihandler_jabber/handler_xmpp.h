#ifndef HANDLER_XMPP_H
#define HANDLER_XMPP_H

#include "abstract_handler.h"

class XmppHandler : public AbstractHandler
{
	virtual qutim_sdk_0_3::Protocol* protocol();
	virtual void open_impl(qutim_sdk_0_3::Account *acc);
	virtual ActionDescription description();
	virtual void newUri(QUrl &uri);
	virtual UriAction action();

	UriAction currentAction_;
	QMap<QString, QString> currentParams_;
};

#endif // HANDLER_XMPP_H
