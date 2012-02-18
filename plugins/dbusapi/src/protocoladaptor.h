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

#ifndef PROTOCOLADAPTOR_H
#define PROTOCOLADAPTOR_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QStringList>
#include <qutim/protocol.h>

using namespace qutim_sdk_0_3;

class ProtocolAdaptor : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.qutim.Protocol")
	Q_PROPERTY(QString id READ id)
public:
    explicit ProtocolAdaptor(const QDBusConnection &dbus, Protocol *parent);
	inline QString id() const { return m_protocol->id(); }
public slots:
	inline QStringList accounts() const { return m_accounts.keys(); }
	inline QDBusObjectPath account(const QString &id) const;
	inline QDBusObjectPath path() const { return m_path; }
signals:
	void accountCreated(const QDBusObjectPath &path, const QString &id);
private slots:
	void onAccountCreated(qutim_sdk_0_3::Account *);
private:
	QDBusConnection m_dbus;
	Protocol *m_protocol;
	QHash<QString, QDBusObjectPath> m_accounts;
	QDBusObjectPath m_path;
};

QDBusObjectPath ProtocolAdaptor::account(const QString &id) const
{
	return m_accounts.value(id, QDBusObjectPath("/"));
}

#endif // PROTOCOLADAPTOR_H

