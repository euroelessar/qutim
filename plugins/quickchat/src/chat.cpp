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
#include "chatchannelmodel.h"
#include "chatpreview.h"
#include "chatcontroller.h"
#include "scriptclient.h"
#include "completionmodel.h"
#include "flatmodel.h"
#include "gradientcreator.h"
#include "selectablemousearea.h"

#include <qutim/thememanager.h>

#include <QQmlEngine>
#include <qqml.h>
#include <QTimer>

namespace QuickChat
{
using namespace qutim_sdk_0_3;

Chat::Chat() :
    m_view(QStringLiteral("quickchat")),
    m_activeSession(0)
{
    init();

    QTimer::singleShot(0, this, [this] () {
        m_view.show();
    });
}

Chat::~Chat()
{
}

void Chat::init()
{
//    qmlRegisterType<MessageUnitData>();
//    qmlRegisterType<Message>();
    qmlRegisterType<QAbstractItemModel>();
    qmlRegisterType<ChatLayer>();
    qmlRegisterType<ChatSession>();
    qmlRegisterType<ChatChannel>();
    qmlRegisterType<ChatController>("org.qutim.quickchat", 0, 4, "ChatController");
    qmlRegisterType<ChatChannelModel>("org.qutim.quickchat", 0, 4, "ChatChannelModel");
    qmlRegisterType<ChatPreview>("org.qutim.quickchat", 0, 4, "ChatPreview");
    qmlRegisterType<CompletionModel>("org.qutim.quickchat", 0, 4, "CompletionModel");
    qmlRegisterType<FlatProxyModel>("org.qutim.quickchat", 0, 4, "FlatProxyModel");
    qmlRegisterType<GradientCreator>("org.qutim.quickchat", 0, 4, "GradientCreator");
    qmlRegisterType<SelectableMouseArea>("org.qutim.quickchat", 0, 4, "SelectableMouseArea");
}

qutim_sdk_0_3::ChatSession *Chat::getSession(qutim_sdk_0_3::ChatUnit *unit, bool create)
{
    // TODO: Think, is it good idea or we need smth more intellegent?
    if (ChatUnit *meta = unit->metaContact())
        unit = meta;

    unit = getUnitForSession(unit);

    if (!unit)
        return nullptr;

	// We don't wont to have separate channels for contact and his resource
	unit = const_cast<ChatUnit*>(unit->getHistoryUnit());
	foreach (ChatChannel *channel, m_channels) {
		if (channel->unit() == unit)
			return channel;
	}
	ChatChannel *channel = 0;
    if (create) {
        m_view.show();

        channel = new ChatChannel(unit);
        QQmlEngine::setObjectOwnership(channel, QQmlEngine::CppOwnership);
        QQmlEngine::setObjectOwnership(unit, QQmlEngine::CppOwnership);
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

QQmlListProperty<ChatChannel> Chat::channels()
{
    QQmlListProperty<ChatChannel> list(this, m_channels);
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
    if (session)
        session->setActive(true);

//	if (session != m_activeSession) {
//		m_activeSession = session;
//		emit activeSessionChanged(session);
//		if (session)
//			session->setActive(true);
//	}
}

void Chat::handleSessionDeath(ChatSession *session)
{
	emit sessionDestroyed(session);
}

void Chat::show()
{
	emit shown();
}

void Chat::onSessionActivated(bool active)
{
    ChatSession *session = static_cast<ChatSession*>(sender());
    qDebug() << session->unit()->title() << active;

    if (active && m_activeSession != session) {
        ChatSession *oldSession = m_activeSession;

        m_activeSession = session;
        emit activeSessionChanged(m_activeSession);

        if (oldSession)
            oldSession->setActive(false);
    } else if (m_activeSession == session && !active) {
        m_activeSession = NULL;
        emit activeSessionChanged(m_activeSession);
    }

    show();
}

void Chat::onSessionDestroyed(QObject *object)
{
	ChatChannel *session = static_cast<ChatChannel*>(object);
	m_channels.removeOne(session);
}

void Chat::forEachChannel(QJSValue callback) const
{
    for (ChatChannel *channel : m_channels) {
        QJSValueList args;
        args << callback.engine()->toScriptValue(channel);
        callback.call(args);
    }
}

}

