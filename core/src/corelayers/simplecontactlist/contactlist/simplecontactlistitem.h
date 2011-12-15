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
#ifndef SIMPLECONTACTLISTITEM_H
#define SIMPLECONTACTLISTITEM_H

#include <qutim/contact.h>
#include <QSharedData>
#include <QModelIndex>
#include <QMetaType>
#include "simplecontactlist_global.h"

using namespace qutim_sdk_0_3;

namespace Core
{
namespace SimpleContactList
{

enum ContactItemType
{
	InvalidType = 0,
	TagType = 100,
	ContactType = 101,
	AccountType = 102
};

enum ContactItemRole
{
	BuddyRole = Qt::UserRole,
	StatusRole,
	ContactsCountRole,
	OnlineContactsCountRole,
	AvatarRole,
	ItemTypeRole,
	AccountRole,
	Color,
	TagName
};
Q_DECLARE_FLAGS(ContactItemRoles,ContactItemRole);

class ItemHelper
{
public:
	ItemHelper(ContactItemType type_) : type(type_) {}
	const ContactItemType type;
};

inline ContactItemType getItemType(const QModelIndex &index)
{ return index.isValid() ? reinterpret_cast<ItemHelper *>(index.internalPointer())->type : InvalidType; }
}

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Core::SimpleContactList::ContactItemRoles);
#endif // SIMPLECONTACTLISTITEM_H

