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

#ifndef FACTORY_H
#define FACTORY_H
#include <qutim/rosterstorage.h>

namespace Core {
namespace MetaContacts {

class Manager;
class Factory : public qutim_sdk_0_3::ContactsFactory
{
	Q_OBJECT
public:
	Factory(Manager *manager);
	virtual ~Factory();
	virtual qutim_sdk_0_3::Contact *addContact(const QString &id, const QVariantMap &data);
	virtual void serialize(qutim_sdk_0_3::Contact *contact, QVariantMap &data);
private:
	Manager *m_manager;
};

} // namespace MetaContacts
} // namespace Core

#endif // FACTORY_H

