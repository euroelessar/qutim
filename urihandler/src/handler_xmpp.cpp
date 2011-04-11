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
	m_currentParams.clear();
	m_currentAction = NotSpecified;
	if (!uri().hasQuery())
		m_currentAction = NotSpecified;
	else if (uri().hasQueryItem("message"))
		m_currentAction = SendMessage;
	else if (uri().hasQueryItem("roster"))
		m_currentAction = AddToRoster;
	else if (uri().hasQueryItem("remove"))
		m_currentAction = RemoveFromRoster;
	else if (uri().hasQueryItem("subscribe"))
		m_currentAction = Subscribe;
	else if (uri().hasQueryItem("unsubscribe"))
		m_currentAction = Unsubscribe;
	else
		m_currentAction = NotSpecified;

	switch (m_currentAction) {
	case SendMessage :
		//currentParams_["subject"] = uri.queryItemValue("subject");
		m_currentParams["body"] = uri().queryItemValue("body");
		//currentParams_["thread"]  = uri.queryItemValue("thread");
		break;
	case AddToRoster :
		m_currentParams["name"]  = uri().queryItemValue("name");
		m_currentParams["group"] = uri().queryItemValue("group");
		break;
	}
	m_currentParams["_contact_id_"] = uri().toString(QUrl::RemoveScheme | QUrl::RemoveQuery);
}

AbstractHandler::UriAction XmppHandler::action()
{
	return m_currentAction;
}

AbstractHandler::ActionDescription XmppHandler::description()
{
	ActionDescription descr;
	if (m_currentParams.value("name").length())
		descr.append(ActionDescriptionItem(QObject::tr("Name of contact is being added"), m_currentParams.value("name")));
	if (m_currentParams.value("group").length())
		descr.append(ActionDescriptionItem(QObject::tr("Group of contact is being added"), m_currentParams.value("group")));
	if (m_currentParams.value("body").length()) {
		QString body = m_currentParams.value("body");
		const int maxBodyLen = 50;
		if (body.length() > maxBodyLen) {
			body.truncate(maxBodyLen);
			body.append("…");
		}
		descr.append(ActionDescriptionItem(QObject::tr("Message to send"), body));
	}
	return descr;
}

void XmppHandler::openImpl(qutim_sdk_0_3::Account *acc)
{
	switch (m_currentAction) {
	default:
	case SendMessage : {
		ChatUnit *unit = acc->getUnit(m_currentParams["_contact_id_"], true);
		Message msg(m_currentParams["body"]);
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
