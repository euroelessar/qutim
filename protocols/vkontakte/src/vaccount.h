/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef VKONTAKTEACCOUNT_H
#define VKONTAKTEACCOUNT_H
#include <qutim/account.h>
#include "vkontakte_global.h"

#include "vclient.h"

class VAccountPrivate;
class VContact;
typedef QList<VContact*> VContactList;
class VConnection;
class VRoster;
class VWallSession;
class LIBVKONTAKTE_EXPORT VAccount : public Account
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VAccount)
public:
	VAccount(const QString& email,QObject *parent = 0);
	virtual VContact* getContact(const QString& uid, bool create = false);
	virtual ChatUnit* getUnit(const QString& unitId, bool create = false);
	QString uid() const;
	virtual QString name() const;
	void setAccountName(const QString &name);
	QString email() const {return id();} //alias for id
	void setUid(const QString &uid);
	virtual void setStatus(Status status);
	virtual ~VAccount();
	VConnection *connection();
	const VConnection *connection() const;
	VContactList contacts() const;
public slots:
	void loadSettings();
	void saveSettings();
protected:
	QString password();
private:
	QScopedPointer<VAccountPrivate> d_ptr;
	friend class VConnection;
	friend class VRosterPrivate;
	friend class VRoster;
};

class VProtocol;
class VClient;
namespace playground {

class VAccount : public qutim_sdk_0_3::Account
{
	Q_OBJECT
public:
	VAccount(const QString &email, VProtocol *protocol);
	VContact *getContact(int uid, bool create = false);

	virtual qutim_sdk_0_3::ChatUnit *getUnit(const QString &unitId, bool create);
	virtual QString name() const;
	virtual void setStatus(Status status);
	void setAccountName(const QString &name);

	int uid() const;
	QString email() const;
public slots:
	void loadSettings();
	void saveSettings();
protected:
	QString requestPassword();
private slots:
	void onClientStateChanged(vk::Client::State);
private:
	VClient *m_client;
	QString m_name;
};

} //namespace playground

#endif // VKONTAKTEACCOUNT_H

