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
class ContactItem;

enum ContactItemType
{
	InvalidType = 0,
	TagType = 100,
	ContactType = 101
};

enum ContactItemRole
{
	BuddyRole = Qt::UserRole,
	StatusRole,
	ContactsCountRole,
	OnlineContactsCountRole,
	AvatarRole,
	ItemTypeRole
};
Q_DECLARE_FLAGS(ContactItemRoles,ContactItemRole);

class SIMPLECONTACTLIST_EXPORT TagItem
{
public:
	inline TagItem() : type(TagType), online(0) {}
	const ContactItemType type;
	QList<ContactItem *> visible;
	int online;
	QString name;
	QList<ContactItem *> contacts;
};

class ContactData : public QSharedData
{
public:
	typedef QExplicitlySharedDataPointer<ContactData> Ptr;
	inline ContactData() {}
	inline ContactData(const ContactData &other)
		: QSharedData(other), contact(other.contact), tags(other.tags), items(other.items) {}
	inline ~ContactData() {}
	Contact *contact;
	QSet<QString> tags;
	QList<ContactItem *> items;
	Status status;
};

class SIMPLECONTACTLIST_EXPORT ContactItem
{
public:
	inline ContactItem() : type(ContactType) {}
	inline ContactItem(const ContactData::Ptr &other_data) : type(ContactType), data(other_data) {}
	inline ContactItem(const ContactItem &other) : type(ContactType), parent(other.parent), data(other.data) {}
	inline int index() const { return parent->contacts.indexOf(const_cast<ContactItem *>(this)); }
	const ContactItemType type;
	TagItem *parent;
	ContactData::Ptr data;
};

class ItemHelper
{
public:
	const ContactItemType type;
};

inline ContactItemType getItemType(const QModelIndex &index)
{ return index.isValid() ? reinterpret_cast<ItemHelper *>(index.internalPointer())->type : InvalidType; }
}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Core::SimpleContactList::ContactItemRoles);
#endif // SIMPLECONTACTLISTITEM_H
