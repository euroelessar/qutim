#include "separatedcontactlistmodel.h"
#include "abstractcontactmodel_p.h"
#include <qutim/metacontact.h>
#include <qutim/metacontactmanager.h>
#include <qutim/mimeobjectdata.h>
#include <qutim/protocol.h>
#include <QMimeData>
#include <QTimer>

namespace Core
{

namespace SimpleContactList
{

class SeparatedModelPrivate : public AbstractContactModelPrivate
{
public:
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
};

inline int ContactItem::parentIndex(void *)
{
	return parent->parent->visibleTags.indexOf(parent);
}

SeparatedModel::SeparatedModel(QObject *parent) : AbstractContactModel(new SeparatedModelPrivate, parent)
{
	Q_D(SeparatedModel);
	d->initData = new SeparatedModelPrivate::InitData;
	QTimer::singleShot(0, this, SLOT(init()));
}

SeparatedModel::~SeparatedModel()
{
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

	if (d->contacts.contains(contact))
		return;

	connect(contact, SIGNAL(destroyed(QObject*)),
			SLOT(contactDeleted(QObject*)));
	connect(contact, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(contactStatusChanged(qutim_sdk_0_3::Status)));
	connect(contact, SIGNAL(nameChanged(QString,QString)),
			SLOT(contactNameChanged(QString)));
	connect(contact, SIGNAL(tagsChanged(QStringList,QStringList)),
			SLOT(contactTagsChanged(QStringList)));
	connect(contact, SIGNAL(inListChanged(bool)),
			SLOT(onContactInListChanged(bool)));

	QStringList tags = contact->tags();
	if(tags.isEmpty())
		tags << tr("Without tags");

	AccountItem *accountItem = d->accountHash.value(contact->account());
	if (!accountItem)
		accountItem = onAccountCreated(contact->account());

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
		item->data->contact->setName(value.toString());
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
		mimeData->setText(item->data->contact->id());
		type = QUTIM_MIME_CONTACT_INTERNAL;
		mimeData->setObject(item->data->contact);
	} else if (itemType == TagType) {
		TagItem *item = reinterpret_cast<TagItem*>(index.internalPointer());
		mimeData->setText(item->name);
		type = QUTIM_MIME_TAG_INTERNAL;
	} else {
		return mimeData;
	}

	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	stream << index.row() << index.column() << qptrdiff(index.internalPointer());
	mimeData->setData(type, encodedData);

	return mimeData;
}

bool SeparatedModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
						 int row, int column, const QModelIndex &parent)
{
	ContactItemType parentType = getItemType(parent);
	if (parentType != ContactType && parentType != TagType)
		return false;

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
	d->unreadContacts.remove(contact);
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
	removeFromContactList(contact,false);
}

void SeparatedModel::contactStatusChanged(Status status)
{
	ContactData::Ptr itemData = d_func()->contacts.value(qobject_cast<Contact *>(sender()));
	updateContactStatus<AccountItem, TagItem, ContactData, ContactItem>(itemData, status);
}

void SeparatedModel::contactNameChanged(const QString &name)
{
	Q_D(SeparatedModel);
	Q_UNUSED(name);
	Contact *contact = qobject_cast<Contact *>(sender());
	ContactData::Ptr item_data = d->contacts.value(contact);
	if(!item_data)
		return;
	const QList<ContactItem *> &items = item_data->items;
	if (items.isEmpty() || !isVisible(items.first()))
		return;
	for(int i = 0; i < items.size(); i++)
		updateContact(items.at(i), true);
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
	Q_D(SeparatedModel);
	AccountItem *item = new AccountItem;
	item->account = account;
	int index = d->accounts.count();
	beginInsertRows(QModelIndex(), index, index);
	d->accounts.push_back(item);
	d->accountHash.insert(account, item);
	endInsertRows();

	foreach (Contact *contact, account->findChildren<Contact*>())
		addContact(contact);
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
	foreach(Protocol *proto, Protocol::all()) {
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach(Account *account, proto->accounts())
			onAccountCreated(account);
	}

	SeparatedModelPrivate::InitData *initData = d->initData;
	d->initData = 0;
	// ensure correct order of tags
	/*QSet<QString> tags;
	foreach (Contact *contact, initData->contacts)
		foreach (const QString &tag, contact->tags())
			tags.insert(tag);
	foreach (const QString &tag, Config().value("contactList/tags", QStringList()))
		if (tags.contains(tag))
			ensureTag<TagItem>(tag);*/
	// add contacts to the contact list
	foreach (Contact *contact, initData->contacts)
		addContact(contact);
	delete initData;
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
		item->data->contact->setTags(tags.toList());
		debug() << "Moving contact from" << item->data->tags << "to" << tags;
	} else if (ev->type == ChangeEvent::MoveTag) {
		/*int to = -2, globalTo = -2;
		if (ev->parent->type == ContactType) {
			TagItem *tag = reinterpret_cast<ContactItem*>(ev->parent)->parent;
			to = p->visibleTags.indexOf(tag) + 1;
			globalTo = p->tags.indexOf(tag) + 1;
		} else if (ev->parent->type == TagType) {
			TagItem *tag = reinterpret_cast<TagItem*>(ev->parent);
			to = p->visibleTags.indexOf(tag);
			globalTo = p->tags.indexOf(tag);
		} else {
			Q_ASSERT(!"Not implemented");
		}
		TagItem *tag = reinterpret_cast<TagItem*>(ev->child);
		int from = p->visibleTags.indexOf(tag);
		int globalFrom = p->tags.indexOf(tag);
		Q_ASSERT(from >= 0 && to >= 0 && globalTo >= 0 && globalFrom >= 0);
		if (beginMoveRows(QModelIndex(), from, from, QModelIndex(), to)) {
			if (from < to) {
				Q_ASSERT(globalFrom < globalTo);
				--to;
				--globalTo;
			}
			p->visibleTags.move(from, to);
			p->tags.move(globalFrom, globalTo);
			endMoveRows();
		}
		saveTagOrder();*/
	}
}

bool SeparatedModel::eventFilter(QObject *obj, QEvent *ev)
{
	/*Q_D(SeparatedModel);
	if (ev->type() == MetaContactChangeEvent::eventType()) {
		MetaContactChangeEvent *metaEvent = static_cast<MetaContactChangeEvent*>(ev);
		if (metaEvent->oldMetaContact() && !metaEvent->newMetaContact())
			addContact(metaEvent->contact());
		else if (!metaEvent->oldMetaContact() && metaEvent->newMetaContact())
			removeContact(metaEvent->contact());
		return false;
	}*/
	return QAbstractItemModel::eventFilter(obj, ev);
}

void SeparatedModel::saveTagOrder()
{
	/*Config group = Config().group("contactList");
	QStringList tags;
	foreach (TagItem *tag, p->tags)
		tags << tag->name;
	group.setValue("tags", tags);*/
}

}
}
