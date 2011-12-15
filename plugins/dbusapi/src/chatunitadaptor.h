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

#ifndef CHATUNITADAPTOR_H
#define CHATUNITADAPTOR_H

#include <qutim/chatunit.h>
#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusConnection>

using namespace qutim_sdk_0_3;

typedef QMap<ChatUnit*, QDBusObjectPath> ChatUnitPathHash;

class ChatUnitAdaptor : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.qutim.ChatUnit")
	Q_PROPERTY(QString id READ id CONSTANT)
	Q_PROPERTY(QString title READ title NOTIFY titleChanged)
	Q_PROPERTY(QDBusObjectPath account READ account)
public:
	static const ChatUnitPathHash &hash();
	static QDBusObjectPath ensurePath(QDBusConnection dbus, ChatUnit *unit);
	
    ChatUnitAdaptor(const QDBusConnection &dbus, const QDBusObjectPath &acc, ChatUnit *unit);
	virtual ~ChatUnitAdaptor();
	inline QString id() const { return m_chatUnit->id(); }
	inline QString title() const { return m_chatUnit->title(); }
	inline QDBusObjectPath account() const { return m_accountPath; }
	inline QDBusObjectPath path() const { return m_path; }
	inline bool sendMessage(const Message &msg) { return m_chatUnit->sendMessage(msg); }
public slots:
	inline qint64 sendMessage(const QString &msg) { return m_chatUnit->sendMessage(msg); }
	QStringList lowerUnits();
	QDBusObjectPath upperUnit();
signals:
	void titleChanged(const QString &current, const QString &previous);
private:
	ChatUnit *m_chatUnit;
	QDBusConnection m_dbus;
	QDBusObjectPath m_path;
	QDBusObjectPath m_accountPath;
};

#endif // CHATUNITADAPTOR_H

