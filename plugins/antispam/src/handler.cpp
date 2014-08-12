/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "handler.h"
#include "settingswidget.h"
#include <qutim/contact.h>
#include <qutim/config.h>
#include <qutim/authorizationdialog.h>
#include <qutim/notification.h>
#include <QTimer>
#include <QDateTime>

namespace Antispam {

using namespace qutim_sdk_0_3;
using namespace Authorization;

#define ANTISPAM_PROPERTY "antispamInfo"

struct Info
{
	typedef QSharedPointer<Info> Ptr;
	
	Info() : trusted(false) {}
	
	bool trusted;
	QDateTime lastQuestionTime;
};

}

Q_DECLARE_METATYPE(Antispam::Info::Ptr)

namespace Antispam {

Handler::Handler() : m_authorization("AuthorizationService")
{
	connect(ServiceManager::instance(), SIGNAL(serviceChanged(QByteArray,QObject*,QObject*)),
	        SLOT(onServiceChanged(QByteArray)));
	loadSettings();
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

	if (m_authorization) {
		if (m_enabled && m_handleAuth)
			m_authorization->installEventFilter(this);
		else
			m_authorization->removeEventFilter(this);
	}
}


MessageHandlerAsyncResult Handler::doHandle(Message &message)
{
	if (!m_enabled || message.property("service", false)) {
		return makeAsyncResult(Accept, QString());
    }

	Contact *contact = qobject_cast<Contact*>(message.chatUnit()->buddy());
	if (!contact || contact->isInList()) {
		return makeAsyncResult(Accept, QString());
    }
	
	Info::Ptr info = contact->property(ANTISPAM_PROPERTY).value<Info::Ptr>();
	if (info.isNull()) {
		info = Info::Ptr::create();
		contact->setProperty(ANTISPAM_PROPERTY, qVariantFromValue(info));
	}
	
	if (info->trusted) {
		return makeAsyncResult(Accept, QString());
    }
	
	if (!message.isIncoming()) {
		if (!message.property("autoreply", false))
			info->trusted = true;
		return makeAsyncResult(Accept, QString());
	}

	//check message body
	foreach (const QString &answer, m_answers) {
		if (message.text().compare(answer, Qt::CaseInsensitive) == 0) {
			Message message(m_success);
			message.setChatUnit(contact);
			contact->sendMessage(message);
			info->trusted = true;
			return makeAsyncResult(Accept, QString());
		}
	}

	if (info->lastQuestionTime.isValid()
	        && qAbs(info->lastQuestionTime.secsTo(QDateTime::currentDateTime())) < 5 * 60) {
		return makeAsyncResult(Reject, QString());
	}
	Message replyMessage(m_question);
	replyMessage.setChatUnit(contact);
	replyMessage.setProperty("autoreply", true);
	contact->sendMessage(replyMessage);
	info->lastQuestionTime = QDateTime::currentDateTime();
	QString reason = tr("Message from %1 blocked on suspicion of spam.").
				   arg(contact->title());

	return makeAsyncResult(Error, reason);
}

bool Handler::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == Reply::eventType()) {
		Authorization::Reply *reply = static_cast<Authorization::Reply*>(event);
		if (reply->replyType() == Reply::New) {
			QString reason;
			Message pseudoMessage(reply->body());
			pseudoMessage.setChatUnit(reply->contact());
			pseudoMessage.setIncoming(false);

            bool accepted = true;

			doHandle(pseudoMessage).connect(this, [&accepted, reply] (Result result, const QString &reason) {
                if (Error == result) {
                    NotificationRequest request(Notification::BlockedMessage);
                    request.setObject(reply->contact());
                    request.setText(reason);
                    request.send();
                }
                accepted = (result == Accept);
            });

			if (accepted)
				return true;
		}
	}
    return QObject::eventFilter(obj, event);
}

void Handler::onServiceChanged(const QByteArray &name)
{
	if (name != m_authorization.name())
		return;
	if (m_enabled && m_handleAuth)
		m_authorization->installEventFilter(this);
}

} // namespace Antispam
