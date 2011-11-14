/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef ACCOUNTADAPTOR_H
#define ACCOUNTADAPTOR_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <qutim/account.h>

using namespace qutim_sdk_0_3;

typedef QMap<Account*, QDBusObjectPath> AccountPathHash;

class AccountAdaptor : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.qutim.Account")
	Q_PROPERTY(QString id READ id)
	Q_PROPERTY(qutim_sdk_0_3::Status status READ status WRITE setStatus NOTIFY statusChanged)
	Q_PROPERTY(QString name READ name NOTIFY nameChanged)
	Q_PROPERTY(QDBusObjectPath protocol READ protocol)
public:
	static const AccountPathHash &hash();
	
    explicit AccountAdaptor(const QDBusConnection &dbus, const QDBusObjectPath &proto, Account *parent);
	virtual ~AccountAdaptor();
	inline QString id() const { return m_account->id(); }
	inline QString name() const { return m_account->name(); }
	inline QDBusObjectPath protocol() const { return m_protocolPath; }
	inline Status status() const { return m_account->status(); }
	inline void setStatus(const Status &status) { m_account->setStatus(status); }
	inline QDBusObjectPath path() const { return m_path; }
public slots:
	QDBusObjectPath chatUnit(const QString &unitId, bool create = false);
	QStringList contacts() const;
signals:
	void nameChanged(const QString &current, const QString &previous);
	void statusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous);
	void contactCreated(const QDBusObjectPath &path, const QString &id);
private slots:
	void onContactCreated(qutim_sdk_0_3::Contact *contact);
	void onConferenceCreated(qutim_sdk_0_3::Conference *conference);
private:
	QDBusConnection m_dbus;
	Account *m_account;
	QDBusObjectPath m_path;
	QDBusObjectPath m_protocolPath;
};

#endif // ACCOUNTADAPTOR_H

