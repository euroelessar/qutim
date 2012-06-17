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
	VRoster(VConnection *connection);
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

namespace vk {
class Roster;
}

namespace playground {

class VRoster : public QObject
{
	Q_OBJECT
public:
	VRoster(vk::Roster *roster);
	VContact *contact(int id);
	VContact *contact(int id) const;
private:
	vk::Roster *m_roster;
};

} //namespace playground

#endif // VROSTER_H

