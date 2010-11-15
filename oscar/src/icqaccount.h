/****************************************************************************
 *  icqaccount.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef ICQACCOUNT_H
#define ICQACCOUNT_H

#include <qutim/account.h>
#include "cookie.h"
#include "capability.h"
#include "oscarstatus.h"
#include <QHostAddress>

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
public:
	IcqAccount(const QString &uin);
	virtual ~IcqAccount();
	virtual void setStatus(Status status);
	void setStatus(OscarStatusEnum status);
	virtual QString name() const;
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
signals:
	void avatarChanged(const QString &avatar);
	void cookieTimeout(const Cookie &cookie);
	void loginFinished();
	void settingsUpdated();
public slots:
	void updateSettings();
private slots:
	void onReconnectTimeout();
	void onPasswordEntered(const QString &password, bool remember);
	void onContactRemoved();
	void onCookieTimeout();
protected:
	void finishLogin();
	virtual bool event(QEvent *ev);
private:
	friend class Roster;
	friend class Cookie;
	friend class OscarConnection;
	friend class BuddyPicture;
	friend class IcqProtocol;
	friend class IcqContact;
	friend class MessagesHandler;
	QScopedPointer<IcqAccountPrivate> d_ptr;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQACCOUNT_H
