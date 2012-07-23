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
#include "separatedcontactlistmodel.h"
#include "qutim/simplecontactlist/abstractcontactmodel_p.h"
#include <qutim/metacontact.h>
#include <qutim/metacontactmanager.h>
#include <qutim/mimeobjectdata.h>
#include <qutim/protocol.h>
#include <qutim/event.h>
#include <qutim/servicemanager.h>
#include <QCoreApplication>
#include <QMimeData>
#include <QTimer>

namespace Core
{

namespace SimpleContactList
{

class SeparatedModelPrivate : public AbstractContactModelPrivate
{
public:
	SeparatedModelPrivate() : initData(0) {}

	QList<AccountItem*> accounts;
	QHash<Account*, AccountItem*> accountHash;
	QMap<Contact *, ContactData::Ptr> contacts;

	struct InitData
	{
		QList<Contact*> contacts;
	};
	// Pointer to variables that are solely used at startup.
	// See SeparatedModel::init()
	InitData *initData;

	quint16 realAccountRequestEvent;
};

inline QModelIndex TagItem::parentIndex(AbstractContactModel *m)
{
	SeparatedModel *model = reinterpret_cast<SeparatedModel*>(m);
	int index = model->d_func()->accounts.indexOf(parent);
	return model->createIndex(index, 0, parent);
}

inline QString TagItem::getName()
{
	return parent->account->id() + "/" + name;
}

inline QModelIndex ContactItem::parentIndex(AbstractContactModel *m)
{
	SeparatedModel *model = reinterpret_cast<SeparatedModel*>(m);
	int row = parent->parent->visibleTags.indexOf(parent);
	return model->createIndex(row, 0, parent);
}

SeparatedModel::SeparatedModel(QObject *parent) : AbstractContactModel(new SeparatedModelPrivate, parent)
{
	Q_D(SeparatedModel);
	d->realAccountRequestEvent = Event::registerType("real-account-request");
	if (!ServiceManager::isInited()) {
		d->initData = new SeparatedModelPrivate::InitData;
		QTimer::singleShot(0, this, SLOT(init()));
	}
	Q_UNUSED(QT_TRANSLATE_NOOP("ContactList", "Show accounts, tags and contacts"));
}

SeparatedModel::~SeparatedModel()
{
}

QList<Contact*> SeparatedModel::contacts() const
{
	return d_func()->contacts.keys();
}

QModelIndex SeparatedModel::index(int row, int, const QModelIndex &parent) const
{
	Q_D(const SeparatedModel);
	if(row < 0)
		return QModelIndex();
	switch(getItemType(parent))
	{
	case AccountType: {
		AccountItem *item = reinterpret_cast<AccountItem *>(parent.internalPointer());
		if(item->tags.size() <= row)
			return QModelIndex();
		return createIndex(row, 0, item->tags.at(row));
	}
	case TagType: {
		TagItem *item = reinterpret_cast<TagItem *>(parent.internalPointer());
		if(item->visible.size() <= row)
			return QModelIndex();
		return createIndex(row, 0, item->visible.at(row));
	}
	case ContactType:
		return QModelIndex();
	default:
		if(d->accounts.size() <= row)
			return QModelIndex();
		return createIndex(row, 0, d->accounts.at(row));
	}
}

QModelIndex SeparatedModel::parent(const QModelIndex &child) const
{
	Q_D(const SeparatedModel);
	switch(getItemType(child))
	{
	case ContactType: {
		ContactItem *item = reinterpret_cast<ContactItem *>(child.internalPointer());
		return createIndex(item->parent->parent->visibleTags.indexOf(item->parent),
						   0, item->parent);
	}
	case TagType: {
		TagItem *item = reinterpret_cast<TagItem *>(child.internalPointer());
		return createIndex(d->accounts.indexOf(item->parent), 0, item->parent);
	} default:
		return QModelIndex();
	}
}

int SeparatedModel::rowCount(const QModelIndex &parent) const
{
	Q_D(const SeparatedModel);
	switch(getItemType(parent))
	{
	case TagType:
		return reinterpret_cast<TagItem *>(parent.internalPointer())->visible.size();
	case ContactType:
		return 0;
	case AccountType:
		return reinterpret_cast<AccountItem *>(parent.internalPointer())->visibleTags.size();
	default:
		return d->accounts.size();
	}
}

bool SeparatedModel::hasChildren(const QModelIndex &parent) const
{
	Q_D(const SeparatedModel);
	switch(getItemType(parent))
	{
	case TagType:
		return !reinterpret_cast<TagItem *>(parent.internalPointer())->visible.isEmpty();
	case ContactType:
		return false;
	case AccountType:
		return !reinterpret_cast<AccountItem *>(parent.internalPointer())->visibleTags.isEmpty();
	default:
		return !d->accounts.isEmpty();
	}
}

QVariant SeparatedModel::data(const QModelIndex &index, int role) const
{
	switch(getItemType(index))
	{
	case ContactType:
		return contactData<ContactItem>(index, role);
	case TagType:
		return tagData<TagItem>(index, role);
	case AccountType:
		return accountData<AccountItem>(index, role);
	default:
		return QVariant();
	}
}

void SeparatedModel::addContact(Contact *contact)
{
	Q_D(SeparatedModel);
	//TODO implement more powerfull logic
	//			if (!contact->isInList())
	//				return;

	if (d->initData) {
		if (d->initData->contacts.contains(contact))
			return;
		d->initData->contacts << contact;
		return;
	}

	Account *account = contact->account();
	Event event(d->realAccountRequestEvent);
	QCoreApplication::sendEvent(account, &event);
	Account *realAccount = event.at<Account*>(0);
	if (realAccount)
		account = realAccount;
	AccountItem *accountItem = d->accountHash.value(account);
	if (!accountItem)
		// This call must be before the following contact existence check
		accountItem = addAccount(account, false);

	if (d->contacts.contains(contact))
		return;

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
	for(QSet<QString>::const_iterator it = item_data->tags.constBegin(); it != item_data->tags.constEnd(); it++)
	{
		TagItem *tag = ensureTag<TagItem>(accountItem, *it);
		ContactItem *item = new ContactItem(item_data);
		item->parent = tag;
		bool show = isVisible(item);
		tag->online += counter;
		if (show) {
			hideContact<AccountItem, TagItem>(item, false, false);
		} else {
			tag->contacts.append(item);
			item_data->items.append(item);
		}
	}
}

bool SeparatedModel::containsContact(Contact *contact) const
{
	return d_func()->contacts.contains(contact);
}

bool SeparatedModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::EditRole && getItemType(index) == ContactType) {
		ContactItem *item = reinterpret_cast<ContactItem *>(index.internalPointer());
		if (item->data->contact)
			item->data->contact.data()->setName(value.toString());
		return true;
	}
	return false;
}

QStringList SeparatedModel::mimeTypes() const
{
	QStringList types;
	types << QUTIM_MIME_CONTACT_INTERNAL;
	types << QUTIM_MIME_TAG_INTERNAL;
	types << MimeObjectData::objectMimeType();
	return types;
}

QMimeData *SeparatedModel::mimeData(const QModelIndexList &indexes) const
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

bool SeparatedModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
						 int row, int column, const QModelIndex &parent)
{
	ContactItemType parentType = getItemType(parent);
	if (parentType != ContactType && parentType != TagType)
		return false;

	if (data->hasFormat(QUTIM_MIME_TAG_INTERNAL)) {
		TagItem *tag = reinterpret_cast<TagItem*>(decodeMimeData(data, QUTIM_MIME_TAG_INTERNAL));
		TagItem *parentTag = 0;
		if (parentType == ContactType)
			parentTag = reinterpret_cast<ContactItem*>(parent.internalPointer())->parent;
		else
			parentTag = reinterpret_cast<TagItem*>(parent.internalPointer());
		Q_ASSERT(tag != 0 && parentTag != 0);
		if (tag->parent != parentTag->parent)
			return false;
	}

	if (parentType == ContactType && data->hasFormat(QUTIM_MIME_CONTACT_INTERNAL))
		return false; // The model does not suppport metacontacts merging yet.

	return AbstractContactModel::dropMimeData(data, action, row, column, parent);
}

QStringList SeparatedModel::tags() const
{
	Q_D(const SeparatedModel);
	QStringList all_tags;
	foreach (const AccountItem *acc, d->accounts)
		foreach (const TagItem *tag, acc->tags)
			all_tags.append(tag->name);
	return all_tags;
}

void SeparatedModel::removeFromContactList(Contact *contact, bool deleted)
{
	Q_D(SeparatedModel);
	Q_UNUSED(deleted);
	ContactData::Ptr item_data = d->contacts.value(contact);
	if(!item_data)
		return;
	int counter = item_data->status.type() == Status::Offline ? 0 : -1;
	for(int i = 0; i < item_data->items.size(); i++) {
		ContactItem *item = item_data->items.at(i);
		item->parent->online += counter;
		hideContact<AccountItem, TagItem>(item, true, false);
		delete item;
	}
	d->contacts.remove(contact);
}

void SeparatedModel::contactDeleted(QObject *obj)
{
	Contact *contact = reinterpret_cast<Contact *>(obj);
	removeFromContactList(contact,true);
}

void SeparatedModel::removeContact(Contact *contact)
{
	Q_ASSERT(contact);
	contact->disconnect(this);
    removeFromContactList(contact, false);
    contactComparator.data()->stopListen(contact);
}

void SeparatedModel::onContactInListChanged(bool)
{
	//Contact *contact = qobject_cast<Contact*>(sender());
	//p->contacts.value(contact)->
	//TODO
}

void SeparatedModel::contactTagsChanged(const QStringList &tags_helper)
{
	Q_D(SeparatedModel);
	Contact *contact = qobject_cast<Contact *>(sender());
	Q_ASSERT(contact);
	ContactData::Ptr item_data = d->contacts.value(contact);
	AccountItem *accountItem = d->accountHash.value(contact->account());
	updateContactTags<AccountItem, TagItem, ContactData, ContactItem>(
			accountItem, item_data, tags_helper);
}

AccountItem *SeparatedModel::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	return addAccount(account, true);
}

AccountItem *SeparatedModel::addAccount(qutim_sdk_0_3::Account *account, bool addContacts)
{
	Q_D(SeparatedModel);
	AccountItem *item = new AccountItem;
	item->account = account;
	item->id = account->id();
	int index = d->accounts.count();
	beginInsertRows(QModelIndex(), index, index);
	d->accounts.push_back(item);
	d->accountHash.insert(account, item);
	endInsertRows();

	if (addContacts) {
		foreach (Contact *contact, account->findChildren<Contact*>())
			addContact(contact);
	}
	connect(account, SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			this, SLOT(addContact(qutim_sdk_0_3::Contact*)));
	connect(account, SIGNAL(destroyed(QObject*)),
			this, SLOT(onAccountDestroyed(QObject*)));
	return item;
}

void SeparatedModel::onAccountDestroyed(QObject *obj)
{
	Q_D(SeparatedModel);
	AccountItem *item = d->accountHash.take(reinterpret_cast<Account*>(obj));
	int index = d->accounts.indexOf(item);
	beginRemoveRows(QModelIndex(), index, index);
	d->accounts.removeAt(index);
	foreach (TagItem *tag, item->tags) {
		foreach (ContactItem *contact, tag->contacts) {
			contact->data->items.removeOne(contact);
			delete contact;
		}
		delete tag;
	}
	endRemoveRows();
}

void SeparatedModel::init()
{
	Q_D(SeparatedModel);
	SeparatedModelPrivate::InitData *initData = d->initData;
	setContacts(initData->contacts);
	delete initData;
}

void SeparatedModel::setContacts(const QList<qutim_sdk_0_3::Contact*> &contacts_helper)
{
	Q_D(SeparatedModel);
	foreach(Protocol *proto, Protocol::all()) {
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach(Account *account, proto->accounts())
			addAccount(account, d->initData);
	}

	QList<qutim_sdk_0_3::Contact*> contacts;
	if (d->initData) {
		contacts = d->initData->contacts;
		d->initData = 0;
	} else {
		contacts = contacts_helper;
	}

	// ensure correct order of tags
	Config cfg = Config().group("contactList");
	foreach (AccountItem *accountItem, d->accounts) {
		Account *account = accountItem->account;
		cfg.beginGroup(account->id());
		QSet<QString> tags;
		foreach (Contact *contact, contacts) {
			if (account != contact->account())
				continue;
			foreach (const QString &tag, contact->tags())
				tags.insert(tag);
		}
		foreach (const QString &tag, cfg.value("tags", QStringList()))
			if (tags.contains(tag))
				ensureTag<TagItem>(accountItem, tag);
		cfg.endGroup();
	}
	// add contacts to the contact list
	foreach (Contact *contact, contacts)
		addContact(contact);
}

void SeparatedModel::filterAllList()
{
	Q_D(SeparatedModel);
	for (int i = 0; i < d->accounts.size(); i++) {
		AccountItem *acc = d->accounts.at(i);
		for (int j = 0; j < acc->tags.size(); j++) {
			TagItem *tag = acc->tags.at(j);
			bool tagFiltered = !d->selectedTags.isEmpty() && !d->selectedTags.contains(tag->name);
			foreach (ContactItem *item, tag->contacts)
				hideContact<AccountItem, TagItem>(item, tagFiltered || !isVisible(item));
		}
	}
}

void SeparatedModel::updateContactData(Contact *contact)
{
	Q_D(SeparatedModel);
	ContactData::Ptr item_data = d->contacts.value(contact);
	if (!item_data)
		return;
	for (int i = 0; i < item_data->items.size(); i++) {
		ContactItem *item = item_data->items.at(i);
		QModelIndex index = createIndex(item->index(), 0, item);
		emit dataChanged(index, index);
	}
}

void SeparatedModel::processEvent(ChangeEvent *ev)
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
	} else if (ev->type == ChangeEvent::MoveTag) {
		moveTag<AccountItem, TagItem, ContactItem>(ev);
		TagItem *tagItem = reinterpret_cast<TagItem*>(ev->child);
		Q_ASSERT(tagItem->type == TagType);
		saveTagOrder(tagItem->parent);
	}
}

bool SeparatedModel::eventFilter(QObject *obj, QEvent *ev)
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

void SeparatedModel::doContactChange(Contact *contact)
{
	ContactData::Ptr itemData = d_func()->contacts.value(contact);
	if(!itemData)
		return;
    updateContactStatus<AccountItem, TagItem, ContactData, ContactItem>(itemData, contact->status());
	const QList<ContactItem *> &items = itemData->items;
    //if (items.isEmpty() || !isVisible(items.first()))
    //	return;
	for(int i = 0; i < items.size(); i++)
		updateContact(items.at(i), true);
}

void SeparatedModel::saveTagOrder(AccountItem *accountItem)
{
	QString groupName = "contactList/" + accountItem->account->id();
	Config group = Config().group(groupName);
	QStringList tags;
	foreach (TagItem *tag, accountItem->tags)
		tags << tag->name;
	group.setValue("tags", tags);
}

}
}

