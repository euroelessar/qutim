/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "chat.h"
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <qutim/thememanager.h>
#include "chatchannelmodel.h"
#include "chatpreview.h"
#include "chatviewstyle.h"
#include "chatcontroller.h"

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

Chat::Chat() : m_activeSession(0)
{
	init();
}

Chat::~Chat()
{
}

void Chat::init()
{
	qRegisterMetaType<Message>();
	qmlRegisterType<ChatLayer>();
	qmlRegisterType<ChatSession>();
	qmlRegisterType<ChatChannel>();
	qmlRegisterType<ChatController>("org.qutim", 0, 3, "ChatController");
	qmlRegisterType<ChatChannelModel>("org.qutim", 0, 3, "ChatChannelModel");
	qmlRegisterType<ChatPreview>("org.qutim", 0, 3, "ChatPreview");
	qmlRegisterUncreatableType<ChatViewStyle>("org.qutim", 0, 3, "ChatViewStyle", "");
}

qutim_sdk_0_3::ChatSession *Chat::getSession(qutim_sdk_0_3::ChatUnit *unit, bool create)
{
	// We don't wont to have separate channels for contact and his resource
	unit = const_cast<ChatUnit*>(unit->getHistoryUnit());
	foreach (ChatChannel *channel, m_channels) {
		if (channel->unit() == unit)
			return channel;
	}
	ChatChannel *channel = 0;
	if (create) {
		channel = new ChatChannel(unit);
		connect(channel, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
		connect(channel, SIGNAL(destroyed(QObject*)), SLOT(onSessionDestroyed(QObject*)));
		m_channels << channel;
		emit channelsChanged(channels());
		emit sessionCreated(channel);
	}
	return channel;
}

QList<qutim_sdk_0_3::ChatSession*> Chat::sessions()
{
	QList<ChatSession*> channels;
	foreach (ChatChannel *channel, m_channels)
		channels << channel;
	return channels;
}

QDeclarativeListProperty<ChatChannel> Chat::channels()
{
	QDeclarativeListProperty<ChatChannel> list(this, m_channels);
	list.append = NULL;
	list.clear = NULL;
	return list;
}

qutim_sdk_0_3::ChatSession *Chat::activeSession() const
{
	return m_activeSession;
}

void Chat::setActiveSession(qutim_sdk_0_3::ChatSession *session)
{
	session->setActive(true);
}

void Chat::show()
{
	emit shown();
}

void Chat::onSessionActivated(bool active)
{
	ChatSession *session = static_cast<ChatSession*>(sender());
	if (active && m_activeSession != session) {
		if (m_activeSession) {
			blockSignals(true);
			m_activeSession->setActive(false);
			blockSignals(false);
		}
		m_activeSession = session;
		emit activeSessionChanged(m_activeSession);
	} else if (m_activeSession == session && !active) {
		m_activeSession = NULL;
		emit activeSessionChanged(m_activeSession);
	}
}

void Chat::onSessionDestroyed(QObject *object)
{
	ChatChannel *session = static_cast<ChatChannel*>(object);
	m_channels.removeOne(session);
}
}

