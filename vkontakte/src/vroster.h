/****************************************************************************
 *  vroster.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef VROSTER_H
#define VROSTER_H

#include <QObject>
#include "vkontakte_global.h"

namespace qutim_sdk_0_3 {
	class Config;
}

class VConnection;
class VContact;
class VRosterPrivate;
class LIBVKONTAKTE_EXPORT VRoster : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VRoster)
public:
	VRoster(VConnection *connection, QObject *parent = 0);
	virtual ~VRoster();
	VContact *getContact(const QString &uid, bool create);
	VContact *getContact(const QVariantMap &data, bool create);
	Config config();
public slots:
	void loadSettings();
	void saveSettings();
	void getProfile();
	void getTagList();
	void getFriendList(); //TODO I think that we need a way to get information on parts
	void setActivity(const Status &activity); //TBD i think that we need a create a separated class
	void requestAvatar(QObject *contact);
	void updateProfile(VContact *contact);
private:
	QScopedPointer<VRosterPrivate> d_ptr;
};

#endif // VROSTER_H
