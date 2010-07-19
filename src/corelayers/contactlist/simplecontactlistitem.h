#ifndef SIMPLECONTACTLISTITEM_H
#define SIMPLECONTACTLISTITEM_H

#include "libqutim/contact.h"
#include <QSharedData>
#include <QModelIndex>
#include <QMetaType>

using namespace qutim_sdk_0_3;

namespace Core
{
	namespace SimpleContactList
	{
		enum ItemDataRole
		{
			ItemDataType = Qt::UserRole ,
			ItemStatusRole,
			ItemContactsCountRole,
			ItemOnlineContactsCountRole
		};

		enum ItemType
		{
			InvalidType = 0,
			TagType = 100,
			ContactType = 101
		};

		class ContactItem;

		class TagItem
		{
		public:
			inline TagItem() : type(TagType), visible(0), online(0) {}
			const ItemType type;
			int visible;
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

		class ContactItem
		{
		public:
			inline ContactItem() : type(ContactType) {}
			inline ContactItem(const ContactData::Ptr &other_data) : type(ContactType), data(other_data) {}
			inline ContactItem(const ContactItem &other) : type(ContactType), parent(other.parent), data(other.data) {}
			inline int index() const { return parent->contacts.indexOf(const_cast<ContactItem *>(this)); }
			const ItemType type;
			TagItem *parent;
			ContactData::Ptr data;
		};

		class ItemHelper
		{
		public:
			const ItemType type;
		};

		inline ItemType getItemType(const QModelIndex &index)
		{ return index.isValid() ? reinterpret_cast<ItemHelper *>(index.internalPointer())->type : InvalidType; }
	}
}

Q_DECLARE_TYPEINFO(Core::SimpleContactList::ItemType, Q_PRIMITIVE_TYPE);
Q_DECLARE_METATYPE(Core::SimpleContactList::ItemType);

#endif // SIMPLECONTACTLISTITEM_H
