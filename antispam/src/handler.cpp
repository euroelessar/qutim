/****************************************************************************
 *  handler.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "handler.h"
#include <qutim/contact.h>
#include <qutim/config.h>
#include "settingswidget.h"
#include <qutim/debug.h>
#include <qutim/servicemanager.h>
#include <qutim/authorizationdialog.h>

#include <QTimer>

namespace Antispam {

using namespace qutim_sdk_0_3;
using namespace Authorization;

Handler::Handler()
{
	QTimer::singleShot(0, this, SLOT(loadSettings()));
}

void Handler::loadSettings()
{
	Config cfg;
	cfg.beginGroup(QLatin1String("antispam"));
	m_enabled = cfg.value("enabled", false);
	m_question = cfg.value("question", tr("Beer, wine, vodka, champagne: after which drink in this sequence I should stop?"));
	m_success = cfg.value("success", tr("We are ready to drink with you!"));
	m_answers = cfg.value("answers", tr("vodka;Vodka")).split(QLatin1String(";"));
	m_handleAuth =  cfg.value("handleAuth", true);
	cfg.endGroup();
	if (QObject *obj = ServiceManager::getByName("AuthorizationService")) {
		if (m_enabled && m_handleAuth)
			obj->installEventFilter(this);
		else
			obj->removeEventFilter(this);
	}
}

MessageHandler::Result Handler::doHandle(Message& message, QString* reason)
{
	if (!message.isIncoming() || !m_enabled)
		return MessageHandler::Accept;

	Contact *contact = qobject_cast<Contact*>(message.chatUnit());
	if (!contact || contact->isInList() || contact->property("trusted").toBool())
		return MessageHandler::Accept;

	//check message body
	foreach (QString answer, m_answers) {
		if (message.text().contains(answer)) {
			Message msg(m_success);
			msg.setChatUnit(contact);
			contact->sendMessage(msg);
			contact->setProperty("trusted", true);
			return MessageHandler::Accept;
		}
	}

	Message msg(m_question);
	msg.setChatUnit(contact);
	contact->sendMessage(msg);
	reason->append(QObject::tr("Message from %1 blocked  on suspicion of spam.").
				   arg(contact->title()));
	return MessageHandler::Error;
}

bool Handler::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == Reply::eventType()) {
		Authorization::Reply *reply = static_cast<Authorization::Reply*>(event);
		Contact *contact = reply->contact();
		bool trusted = contact->property("trusted").toBool();
		if (reply->replyType() == Reply::New && !trusted) {
			Message msg(m_question);
			msg.setChatUnit(contact);
			contact->sendMessage(msg);
			return true;
		}
	}
	return QObject::eventFilter(obj, event);
}

} // namespace Antispam
