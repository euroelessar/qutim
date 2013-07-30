/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef BIRTHDAYREMINDER_H
#define BIRTHDAYREMINDER_H

#include <qutim/plugin.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/contact.h>
#include <qutim/inforequest.h>
#include <QTimer>

using namespace qutim_sdk_0_3;

class BirthdayUpdater : public QObject
{
	Q_OBJECT
public:
	BirthdayUpdater(Account *account, InfoRequestFactory *factory, QObject *parent = 0);
	void update(Contact *contact);
signals:
	void birthdayUpdated(Contact *contact, const QDate &birthday);
private slots:
	void onUpdateNext();
	void onStatusChanged(const qutim_sdk_0_3::Status &current,
						 const qutim_sdk_0_3::Status &previous);
	void onRequestStateChanged(qutim_sdk_0_3::InfoRequest::State state);
private:
	Account *m_account;
	InfoRequestFactory *m_factory;
	QList<QPointer<Contact> > m_waitingUpdate;
	quint8 m_updateFails;
	QTimer m_updateTimer;
};

class BirthdayReminder : public Plugin
{
	Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qutim.Plugin")

public:
	BirthdayReminder();
	virtual void init();
	virtual bool load();
	virtual bool unload();
private slots:
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountDestroyed(QObject *obj);
	void onContactCreated(qutim_sdk_0_3::Contact *contact);
	void onBirthdayUpdated(Contact *contact, const QDate &birthday);
	void onNotificationTimeout();
	void reloadSettings();
private:
	void checkContactBirthday(Contact *contact, const QDate &birthday, const QDate &current);
	void checkAccount(Account *account, BirthdayUpdater *updater, InfoRequestFactory *factory);
	void checkContact(Contact *contact, BirthdayUpdater *updater, InfoRequestFactory *factory,
					  Config &cfg, const QDate &currentDate);
private:
	QHash<Account*, BirthdayUpdater*> m_accounts;
	QTimer m_notificationTimer;
	quint8 m_daysBeforeNotification;
	SettingsItem *m_settings;
};

#endif

