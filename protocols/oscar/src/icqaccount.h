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

#ifndef ICQACCOUNT_H
#define ICQACCOUNT_H

#include <qutim/account.h>
#include "cookie.h"
#include "capability.h"
#include "oscarstatus.h"
#include <QHostAddress>
#include <QNetworkProxy>

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqAccountPrivate;
class RosterPlugin;
class Feedbag;
class AbstractConnection;

class LIBOSCAR_EXPORT IcqAccount: public Account
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(IcqAccount)
	Q_PROPERTY(QString avatar WRITE setAvatar READ avatar NOTIFY avatarChanged)
	Q_PROPERTY(bool htmlEnabled READ isHtmlEnabled WRITE setHtmlEnabled NOTIFY htmlEnabledChanged)
public:
	IcqAccount(const QString &uin);
	virtual ~IcqAccount();
	virtual void setStatus(Status status);
	void setStatus(OscarStatusEnum status);
	virtual QString name() const;
	void setName(const QString &name);
	QString avatar() const;
	void setAvatar(const QString &avatar);
	Feedbag *feedbag();
	AbstractConnection *connection();
	const AbstractConnection *connection() const;
	ChatUnit *getUnit(const QString &unitId, bool create = false);
	IcqContact *getContact(const QString &id, bool create = false, bool forceCreating = false);
	const QHash<QString, IcqContact*> &contacts() const;
	void setCapability(const Capability &capability, const QString &type = QString());
	bool removeCapability(const Capability &capability);
	bool removeCapability(const QString &type);
	bool containsCapability(const Capability &capability) const;
	bool containsCapability(const QString &type) const;
	QList<Capability> capabilities() const;
	void registerRosterPlugin(RosterPlugin *plugin);
	void setProxy(const QNetworkProxy &proxy);
	bool isHtmlEnabled() const;

signals:
	void avatarChanged(const QString &avatar);
	void loginFinished();
	void settingsUpdated();
	void proxyUpdated(const QNetworkProxy &proxy);
	void htmlEnabledChanged(bool htmlEnabled);

public slots:
	void updateSettings();
	void setHtmlEnabled(bool htmlEnabled);

private slots:
	void onPasswordEntered(const QString &password, bool remember);
	void onContactRemoved();
	void onCookieTimeout();
protected:
	void finishLogin();
private:
	friend class Roster;
	friend class Cookie;
	friend class OscarConnection;
	friend class BuddyPicture;
	friend class IcqProtocol;
	friend class IcqContact;
	friend class MessagesHandler;
	QScopedPointer<IcqAccountPrivate> d_ptr;
	bool m_htmlEnabled;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQACCOUNT_H

