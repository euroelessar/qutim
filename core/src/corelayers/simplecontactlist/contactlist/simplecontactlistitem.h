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
