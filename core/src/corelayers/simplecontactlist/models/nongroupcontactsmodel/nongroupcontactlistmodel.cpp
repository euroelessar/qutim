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
#include "nongroupcontactlistmodel.h"
#include "qutim/simplecontactlist/abstractcontactmodel_p.h"
#include <qutim/metacontact.h>
#include <qutim/metacontactmanager.h>
#include <qutim/mimeobjectdata.h>
#include <qutim/protocol.h>
#include <qutim/servicemanager.h>
#include <QMimeData>
#include <QMessageBox>
#include <QTimer>

namespace Core
{
namespace SimpleContactList
{

class NonGroupModelPrivate : public AbstractContactModelPrivate
{
public:
	NonGroupModelPrivate() : initData(0) {}

	QList<TagItem *> tags;
	QList<TagItem *> visibleTags;
	QHash<QString, TagItem *> tagsHash;
	QMap<Contact *, ContactData::Ptr> contacts;

	struct InitData
	{
		QList<Contact*> contacts;
	};
	// Pointer to variables that are solely used at startup.
	// See NonGroupModel::init()
	InitData *initData;
};

inline NonGroupModelPrivate *TagItem::getTagContainer(AbstractContactModel *m)
{
	return reinterpret_cast<NonGroupModel*>(m)->d_func();
}

inline QModelIndex ContactItem::parentIndex(AbstractContactModel *m)
{
	NonGroupModel *model = reinterpret_cast<NonGroupModel*>(m);
	int row = model->d_func()->visibleTags.indexOf(parent);
	return model->createIndex(row, 0, parent);
}

NonGroupModel::NonGroupModel(QObject *parent) : AbstractContactModel(new NonGroupModelPrivate, parent)
{
	Q_D(NonGroupModel);
	if (!ServiceManager::isInited()) {
		d->initData = new NonGroupModelPrivate::InitData;
		QTimer::singleShot(0, this, SLOT(init()));
	}
	if (1) {} else Q_UNUSED(QT_TRANSLATE_NOOP("ContactList", "Show tags and contacts"));
}

NonGroupModel::~NonGroupModel()
{
}

QList<Contact*> NonGroupModel::contacts() const
{
	return d_func()->contacts.keys();
}

QModelIndex NonGroupModel::index(int row, int, const QModelIndex &parent) const
{
	Q_D(const NonGroupModel);
	if(row < 0)
		return QModelIndex();
	switch(getItemType(parent))
	{
	case TagType: {
		TagItem *item = reinterpret_cast<TagItem *>(parent.internalPointer());
		if(item->visible.size() <= row)
			return QModelIndex();
		return createIndex(row, 0, item->visible.at(row));
	}
	case ContactType:
		return QModelIndex();
	default:
		if(d->visibleTags.size() <= row)
			return QModelIndex();
		return createIndex(row, 0, d->visibleTags.at(row));
	}
}

QModelIndex NonGroupModel::parent(const QModelIndex &child) const
{
	Q_D(const NonGroupModel);
	switch(getItemType(child))
	{
	case ContactType: {
		ContactItem *item = reinterpret_cast<ContactItem *>(child.internalPointer());
		return createIndex(d->visibleTags.indexOf(item->parent), 0, item->parent);
	}
	case TagType:
	default:
		return QModelIndex();
	}
}

int NonGroupModel::rowCount(const QModelIndex &parent) const
{
	Q_D(const NonGroupModel);
	switch(getItemType(parent))
	{
	case TagType:
		return reinterpret_cast<TagItem *>(parent.internalPointer())->visible.size();
	case ContactType:
		return 0;
	default:
		return d->visibleTags.size();
	}
}

bool NonGroupModel::hasChildren(const QModelIndex &parent) const
{
	Q_D(const NonGroupModel);
	switch(getItemType(parent))
	{
	case TagType:
		return !reinterpret_cast<TagItem *>(parent.internalPointer())->visible.isEmpty();
	case ContactType:
		return false;
	default:
		return !d->visibleTags.isEmpty();
	}
}

QVariant NonGroupModel::data(const QModelIndex &index, int role) const
{
	switch(getItemType(index))
	{
	case ContactType:
		return contactData<ContactItem>(index, role);
	case TagType:
		return tagData<TagItem>(index, role);
	default:
		return QVariant();
	}
}

void NonGroupModel::addContact(Contact *contact)
{
	Q_D(NonGroupModel);
	//TODO implement more powerfull logic
	//			if (!contact->isInList())
	//				return;

	if (d->initData) {
		if (d->initData->contacts.contains(contact))
			return;
		d->initData->contacts << contact;
		return;
	}

	if (d->contacts.contains(contact))
		return;

	MetaContact *meta = qobject_cast<MetaContact*>(contact);

	if (!meta)
		meta = static_cast<MetaContact*>(contact->metaContact());

	if (meta && d->contacts.contains(meta))
		return;
	else if (meta)
		contact = meta;

	if (meta) {
		meta->installEventFilter(this);
		foreach (ChatUnit *unit, meta->lowerUnits()) {
			Contact *subContact = qobject_cast<Contact*>(unit);
			if (subContact && d->contacts.contains(subContact))
				removeContact(subContact);
		}
	}

	connect(contact, SIGNAL(destroyed(QObject*)),
			SLOT(contactDeleted(QObject*)));
	connect(contact, SIGNAL(tagsChanged(QStringList,QStringList)),
			SLOT(contactTagsChanged(QStringList)));
	connect(contact, SIGNAL(inListChanged(bool)),
			SLOT(onContactInListChanged(bool)));
	contactComparator.data()->startListen(contact);

	QStringList tags = contact->tags();
	if(tags.isEmpty())
		tags << tr("Without tags");

	ContactData::Ptr item_data(new ContactData);
	item_data->contact = contact;
	item_data->tags = QSet<QString>::fromList(tags);
	item_data->status = contact->status();
	int counter = item_data->status.type() == Status::Offline ? 0 : 1;
	d->contacts.insert(contact, item_data);
	
	TagItem *tag = contact->isInList() ? inListTag() : notInListTag();
	ContactItem *item = new ContactItem(item_data);
	item->parent = tag;
	bool show = isVisible(item);
	tag->online += counter;
	if (show) {
		hideContact<NonGroupModelPrivate, TagItem>(item, false, false);
	} else {
		tag->contacts.append(item);
		item_data->items.append(item);
	}
}

bool NonGroupModel::containsContact(Contact *contact) const
{
	return d_func()->contacts.contains(contact);
}

bool NonGroupModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::EditRole && getItemType(index) == ContactType) {
		ContactItem *item = reinterpret_cast<ContactItem *>(index.internalPointer());
		if (item->data->contact)
			item->data->contact.data()->setName(value.toString());
		return true;
	}
	return false;
}

QStringList NonGroupModel::mimeTypes() const
{
	QStringList types;
	types << QUTIM_MIME_CONTACT_INTERNAL;
	types << QUTIM_MIME_TAG_INTERNAL;
	types << MimeObjectData::objectMimeType();
	return types;
}

QMimeData *NonGroupModel::mimeData(const QModelIndexList &indexes) const
{
	MimeObjectData *mimeData = new MimeObjectData();
	QModelIndex index = indexes.value(0);
	ContactItemType itemType = getItemType(index);
	QLatin1String type("");
	if (itemType == ContactType) {
		ContactItem *item = reinterpret_cast<ContactItem*>(index.internalPointer());
		if (item->data->contact) {
			Contact *contact = item->data->contact.data();
			mimeData->setText(contact->id());
			type = QUTIM_MIME_CONTACT_INTERNAL;
			mimeData->setObject(contact);
		}
	} else if (itemType == TagType) {
		TagItem *item = reinterpret_cast<TagItem*>(index.internalPointer());
		mimeData->setText(item->name);
		type = QUTIM_MIME_TAG_INTERNAL;
	} else {
		return mimeData;
	}

	setEncodedData(mimeData, type, index);

	return mimeData;
}

bool NonGroupModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
						 int row, int column, const QModelIndex &parent)
{
	ContactItemType parentType = getItemType(parent);
	if (parentType != ContactType)
		return false;
	return AbstractContactModel::dropMimeData(data, action, row, column, parent);
}

void NonGroupModel::removeFromContactList(Contact *contact, bool deleted)
{
	Q_D(NonGroupModel);
	Q_UNUSED(deleted);
	ContactData::Ptr item_data = d->contacts.value(contact);
	if(!item_data)
		return;
	int counter = item_data->status.type() == Status::Offline ? 0 : -1;
	for(int i = 0; i < item_data->items.size(); i++) {
		ContactItem *item = item_data->items.at(i);
		item->parent->online += counter;
		hideContact<NonGroupModelPrivate, TagItem>(item, true, false);
		delete item;
	}
	d->contacts.remove(contact);
}

void NonGroupModel::contactDeleted(QObject *obj)
{
	Contact *contact = reinterpret_cast<Contact *>(obj);
	removeFromContactList(contact,true);
}

void NonGroupModel::removeContact(Contact *contact)
{
	Q_D(NonGroupModel);
	Q_ASSERT(contact);
	if (MetaContact *meta = qobject_cast<MetaContact*>(contact)) {
		contact->removeEventFilter(this);
		foreach (ChatUnit *unit, meta->lowerUnits()) {
			Contact *subContact = qobject_cast<Contact*>(unit);
			if (subContact && !d->contacts.contains(subContact))
				addContact(subContact);
		}
	}
	contact->disconnect(this);
	removeFromContactList(contact,false);
	contactComparator.data()->stopListen(contact);
}

void NonGroupModel::onContactInListChanged(bool inList)
{
	Q_D(NonGroupModel);
	Contact *contact = qobject_cast<Contact *>(sender());
	Q_ASSERT(contact);
	ContactData::Ptr item_data = d->contacts.value(contact);
	TagItem *tag = inList ? inListTag() : notInListTag();
	updateContactTags<NonGroupModelPrivate, TagItem, ContactData, ContactItem>(
			d, item_data, QStringList(tag->name));
}

void NonGroupModel::contactTagsChanged(const QStringList &tags_helper)
{
	Q_UNUSED(tags_helper);
//	Q_D(NonGroupModel);
//	Contact *contact = qobject_cast<Contact *>(sender());
//	Q_ASSERT(contact);
//	ContactData::Ptr item_data = d->contacts.value(contact);
//	updateContactTags<NonGroupModelPrivate, TagItem, ContactData, ContactItem>(
//			d, item_data, tags_helper);
}

QStringList NonGroupModel::tags() const
{
	Q_D(const NonGroupModel);
	QStringList all_tags;
	foreach (const TagItem *tag,d->tags)
		all_tags.append(tag->name);
	return all_tags;
}

void NonGroupModel::init()
{
	Q_D(NonGroupModel);
	NonGroupModelPrivate::InitData *initData = d->initData;
	setContacts(initData->contacts);
	delete initData;
}

TagItem *NonGroupModel::inListTag()
{
	return ensureTag<TagItem>(d_func(), tr("In list"));
}

TagItem *NonGroupModel::notInListTag()
{
	return ensureTag<TagItem>(d_func(), tr("Not in list"));
}

void NonGroupModel::setContacts(const QList<qutim_sdk_0_3::Contact*> &contacts_helper)
{
	Q_D(NonGroupModel);
	foreach(Protocol *proto, Protocol::all()) {
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach(Account *account, proto->accounts())
			onAccountCreated(account);
	}

	QList<qutim_sdk_0_3::Contact*> contacts;
	if (d->initData) {
		contacts = d->initData->contacts;
		d->initData = 0;
	} else {
		contacts = contacts_helper;
	}

	inListTag();
	notInListTag();

	// add contacts to the contact list
	foreach (Contact *contact, contacts)
		addContact(contact);
}

void NonGroupModel::filterAllList()
{
	Q_D(NonGroupModel);
	for (int i = 0; i < d->tags.size(); i++) {
		TagItem *tag = d->tags.at(i);
		bool tagFiltered = !d->selectedTags.isEmpty() && !d->selectedTags.contains(tag->name);
		foreach (ContactItem *item, tag->contacts)
			hideContact<NonGroupModelPrivate, TagItem>(item, tagFiltered || !isVisible(item));
	}
}

void NonGroupModel::updateContactData(Contact *contact)
{
	Q_D(NonGroupModel);;
	ContactData::Ptr item_data = d->contacts.value(contact);
	if (!item_data)
		return;
	for (int i = 0; i < item_data->items.size(); i++) {
		ContactItem *item = item_data->items.at(i);
		QModelIndex index = createIndex(item->index(), 0, item);
		emit dataChanged(index, index);
	}
}

void NonGroupModel::processEvent(ChangeEvent *ev)
{
	ContactItem *item = reinterpret_cast<ContactItem*>(ev->child);
	if (ev->type == ChangeEvent::ChangeTags) {
		TagItem *tag = reinterpret_cast<TagItem*>(ev->parent);
		if (tag->name == item->parent->name)
			return;

		QSet<QString> tags = item->data->tags;
		tags.remove(item->parent->name);
		tags.insert(tag->name);
		if (item->data->contact)
			item->data->contact.data()->setTags(tags.toList());
		debug() << "Moving contact from" << item->data->tags << "to" << tags;
	} else if (ev->type == ChangeEvent::MergeContacts) { // MetaContacts
		ContactItem *parent = reinterpret_cast<ContactItem*>(ev->parent);
		showContactMergeDialog(parent, item);
	} else if (ev->type == ChangeEvent::MoveTag) {
		moveTag<NonGroupModelPrivate, TagItem, ContactItem>(ev);
		saveTagOrder();
	}
}

bool NonGroupModel::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == MetaContactChangeEvent::eventType()) {
		MetaContactChangeEvent *metaEvent = static_cast<MetaContactChangeEvent*>(ev);
		if (metaEvent->oldMetaContact() && !metaEvent->newMetaContact())
			addContact(metaEvent->contact());
		else if (!metaEvent->oldMetaContact() && metaEvent->newMetaContact())
			removeContact(metaEvent->contact());
		return false;
	}
	return QAbstractItemModel::eventFilter(obj, ev);
}

void NonGroupModel::doContactChange(Contact *contact)
{
	Q_D(NonGroupModel);
	ContactData::Ptr itemData = d->contacts.value(contact);
	if (!itemData)
		return;
    updateContactStatus<NonGroupModelPrivate, TagItem, ContactData, ContactItem>(itemData, contact->status());
	const QList<ContactItem *> &items = itemData->items;
    //if (items.isEmpty() || !isVisible(items.first()))
    //	return;
	for(int i = 0; i < items.size(); i++)
		updateContact(items.at(i), true);
}

void NonGroupModel::saveTagOrder()
{
	Q_D(NonGroupModel);
	Config group = Config().group("contactList");
	QStringList tags;
	foreach (TagItem *tag, d->tags)
		tags << tag->name;
	group.setValue("tags", tags);
}

void NonGroupModel::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	foreach (Contact *contact, account->findChildren<Contact*>())
		addContact(contact);
	addAccount(account);
}

void NonGroupModel::addAccount(qutim_sdk_0_3::Account *account)
{
	connect(account, SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			this, SLOT(addContact(qutim_sdk_0_3::Contact*)));
}

}
}


