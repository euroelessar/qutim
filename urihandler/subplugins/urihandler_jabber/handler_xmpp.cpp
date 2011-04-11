#include "handler_xmpp.h"
#include "urihandlerservice.h"

#include <QUrl>
#include <QMessageBox>

#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/message.h>
#include <qutim/chatsession.h>

using namespace qutim_sdk_0_3;

Protocol *XmppHandler::protocol()
{
	return Protocol::all().value(QLatin1String("jabber"));
}

void XmppHandler::newUri(QUrl &)
{
	currentParams_.clear();
	currentAction_ = NotSpecified;
	if (!uri().hasQuery())
		currentAction_ = NotSpecified;
	else if (uri().hasQueryItem("message"))
		currentAction_ = SendMessage;
	else if (uri().hasQueryItem("roster"))
		currentAction_ = AddToRoster;
	else if (uri().hasQueryItem("remove"))
		currentAction_ = RemoveFromRoster;
	else if (uri().hasQueryItem("subscribe"))
		currentAction_ = Subscribe;
	else if (uri().hasQueryItem("unsubscribe"))
		currentAction_ = Unsubscribe;
	else
		currentAction_ = NotSpecified;

	switch (currentAction_) {
	case SendMessage :
		//currentParams_["subject"] = uri.queryItemValue("subject");
		currentParams_["body"] = uri().queryItemValue("body");
		//currentParams_["thread"]  = uri.queryItemValue("thread");
		break;
	case AddToRoster :
		currentParams_["name"]  = uri().queryItemValue("name");
		currentParams_["group"] = uri().queryItemValue("group");
		break;
	}
	currentParams_["_contact_id_"] = uri().toString(QUrl::RemoveScheme | QUrl::RemoveQuery);
}

AbstractHandler::UriAction XmppHandler::action()
{
	return currentAction_;
}

AbstractHandler::ActionDescription XmppHandler::description()
{
	ActionDescription descr;
	if (currentParams_.value("name").length())
		descr.append(ActionDescriptionItem(QObject::tr("Name of contact is being added"), currentParams_.value("name")));
	if (currentParams_.value("group").length())
		descr.append(ActionDescriptionItem(QObject::tr("Group of contact is being added"), currentParams_.value("group")));
	if (currentParams_.value("body").length()) {
		QString body = currentParams_.value("body");
		const int maxBodyLen = 50;
		if (body.length() > maxBodyLen) {
			body.truncate(maxBodyLen);
			body.append("…");
		}
		descr.append(ActionDescriptionItem(QObject::tr("Message to send"), body));
	}
	return descr;
}

void XmppHandler::open_impl(qutim_sdk_0_3::Account *acc)
{
	switch (currentAction_) {
	default:
	case SendMessage : {
		ChatUnit *unit = acc->getUnit(currentParams_["_contact_id_"], true);
		Message msg(currentParams_["body"]);
		msg.setChatUnit(unit);
		msg.setIncoming(false);
		unit->sendMessage(msg);
		ChatSession *sess = ChatLayer::instance()->getSession(unit);
		sess->activate();
		break;
	}
	case AddToRoster :
	case RemoveFromRoster:
	case Subscribe :
	case Unsubscribe:
		QMessageBox::information(0, "qutIM", "Not implemented. :(");
	}
}

XmppHandler instance;
