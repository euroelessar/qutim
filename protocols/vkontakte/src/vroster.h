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
#include <QHash>

namespace vk {
class Roster;
}
class VAccount;
class VContact;
class VContactsFactory;

class VRoster : public QObject
{
	Q_OBJECT
public:
	VRoster(VAccount *account);
	VContact *contact(const QString &id);
	VContact *contact(const QString &id) const;
private slots:
	void onContactDestroyed(QObject *obj);
private:
	VAccount *m_account;
	vk::Roster *m_roster;
	QHash<QString, VContact*> m_contactHash;
};


#endif // VROSTER_H

