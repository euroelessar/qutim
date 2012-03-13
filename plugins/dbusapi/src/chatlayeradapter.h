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

#ifndef CHATLAYERADAPTER_H
#define CHATLAYERADAPTER_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <qutim/chatsession.h>

using namespace qutim_sdk_0_3;

class ChatLayerAdapter : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.qutim.ChatLayer")
public:
    explicit ChatLayerAdapter(const QDBusConnection &dbus);
public slots:
	QDBusObjectPath session(const QDBusObjectPath &chatUnit, bool create = true);
	QDBusObjectPath session(const QDBusObjectPath &account, const QString &id, bool create = true);
	QList<QDBusObjectPath> sessions() const;
signals:
	void sessionCreated(const QDBusObjectPath &sessionPath);
private slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
private:
	QDBusConnection m_dbus;
	int m_lastId;
};

#endif // CHATLAYERADAPTER_H

