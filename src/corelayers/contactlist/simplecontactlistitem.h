#ifndef SIMPLECONTACTLISTITEM_H
#define SIMPLECONTACTLISTITEM_H

#include <qutim/contact.h>
#include <QSharedData>
#include <QModelIndex>
#include <QMetaType>
#include <contactdelegate.h>

using namespace qutim_sdk_0_3;

namespace Core
{
	namespace SimpleContactList
	{
		class ContactItem;

		class TagItem
		{
		public:
			inline TagItem() : type(TagType), visible(0), online(0) {}
			const ContactItemType type;
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

#endif // SIMPLECONTACTLISTITEM_H
