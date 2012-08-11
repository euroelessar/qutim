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

#include "vclient.h"
#include <QPointer>

class QWebPage;
class VContact;
typedef QList<VContact*> VContactList;
class VProtocol;
class VClient;
class VRoster;

class VAccount : public qutim_sdk_0_3::Account
{
	Q_OBJECT
	Q_PROPERTY(Vreen::Client* client READ client CONSTANT)
public:
	VAccount(const QString &email, VProtocol *protocol);
	VContact *getContact(int uid, bool create = false);

	virtual qutim_sdk_0_3::ChatUnit *getUnit(const QString &unitId, bool create);
	virtual QString name() const;
	virtual void setStatus(qutim_sdk_0_3::Status status);

	int uid() const;
	QString email() const;
	Vreen::Connection *connection() const;
	Vreen::Client *client() const;
	VContact *me() const;
public slots:
	void loadSettings();
	void saveSettings();
protected:
	VRoster *roster() const;
	VRoster *roster();
private slots:
	void onClientStateChanged(Vreen::Client::State);
	void onMeChanged(Vreen::Buddy *me);
	void onInvisibleChanged(bool set);
	void onAuthConfirmRequested(QWebPage *page);
	void setAccessToken(const QByteArray &token, time_t expiresIn);
	void onError(Vreen::Client::Error);
private:
	VClient *m_client;
	QPointer<VRoster> m_roster;
	QPointer<VContact> m_me;
	QString m_name;

	friend class VRoster;
};

#endif // VKONTAKTEACCOUNT_H

