/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef VROSTER_P_H
#define VROSTER_P_H
#include <QObject>
#include "vkontakte_global.h"
#include <QTimer>
#include <QDateTime>
#include <QQueue>
#include <qutim/rosterstorage.h>

class VConnection;
class VRoster;
class VContact;
class VRosterPrivate : public QObject, public ContactsFactory
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(VRoster)
public:
	VConnection *connection;
	VRoster *q_ptr;
	RosterStorage *storage;
	int limit;
	int friendListUpdateInterval;
	QTimer friendListUpdater;
	QTimer avatarsUpdater;
	QTimer activityUpdater;
	QQueue<QObject*> avatarsQueue;
	bool fetchAvatars;
	bool getActivity;
	QMap<QString, QString> tags;
	QHash<QString, VContact*> contacts;
	QDateTime lastActivityTime;
	
	void checkPhoto(QObject *obj, const QString &photoUrl);
	QString photoHash(const QString &path);
	Contact *addContact(const QString &id, const QVariantMap &data);
	void serialize(Contact *contact, QVariantMap &data);
public slots:
	void onGetProfileRequestFinished(const QVariant &var, bool error);
	void onGetTagListRequestFinished(const QVariant &var, bool error);
	void onGetFriendsRequestFinished(const QVariant &var, bool error);
	void onConnectStateChanged(VConnectionState state);
	void onAvatarRequestFinished();
	void onActivityUpdateRequestFinished(const QVariant &var, bool error);
	void onSetActivityRequestFinished(const QVariant &var, bool error);
	void onUpdateProfileFinished(const QVariant &var, bool error);
	void updateAvatar();
	void updateActivity();
	void onContactDestroyed(QObject *obj);
};

#endif // VROSTER_P_H

