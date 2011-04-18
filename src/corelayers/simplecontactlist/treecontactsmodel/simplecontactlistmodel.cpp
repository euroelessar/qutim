#include "simplecontactlistmodel.h"
#include "abstractcontactmodel_p.h"
#include <qutim/notificationslayer.h>
#include <QDebug>
#include <qutim/messagesession.h>
#include <qutim/status.h>
#include <QTreeView>
#include <qutim/icon.h>
#include <qutim/configbase.h>
#include <qutim/debug.h>
#include <qutim/metacontact.h>
#include <qutim/metacontactmanager.h>
#include <QBasicTimer>
#include <QMimeData>
#include <QMessageBox>
#include <qutim/mimeobjectdata.h>
#include <qutim/event.h>
#include <QApplication>
#include <qutim/systemintegration.h>
#include <qutim/protocol.h>
#include <QTimer>

#define QUTIM_MIME_CONTACT_INTERNAL QLatin1String("application/qutim-contact-internal")
#define QUTIM_MIME_TAG_INTERNAL QLatin1String("application/qutim-tag-internal")

namespace Core
{
namespace SimpleContactList
{
struct ChangeEvent
{
	enum Type { ChangeTags, MergeContacts, MoveTag } type;
	void *child;
	ItemHelper *parent;

};

class TreeModelPrivate : public AbstractContactModelPrivate
{
public:
	QList<TagItem *> tags;
	QList<TagItem *> visibleTags;
	QHash<QString, TagItem *> tagsHash;
	QMap<Contact *, ContactData::Ptr> contacts;
	QList<ChangeEvent*> events;
	QBasicTimer timer;
	QString lastFilter;
	QStringList selectedTags;
	bool showOffline;
	QBasicTimer unreadTimer;
	QMap<ChatSession*, QSet<Contact*> > unreadBySession;
	QSet<Contact*> unreadContacts;
	bool showMessageIcon;
	QIcon unreadIcon;
	quint16 realUnitRequestEvent;

	struct InitData
	{
		QList<Contact*> contacts;
		quint16 qutimStartupEvent;
	};
	// Pointer to variables that are solely used at startup.
	// See Model::initialize()
	InitData *initData;
};

TreeModel::TreeModel(QObject *parent) : AbstractContactModel(new TreeModelPrivate, parent)
{
	Q_D(TreeModel);
	d->showMessageIcon = false;
	Event::eventManager()->installEventFilter(this);
	d->initData = new TreeModelPrivate::InitData;
	d->realUnitRequestEvent = Event::registerType("real-chatunit-request");
	d->initData->qutimStartupEvent = Event::registerType("startup");
	d->unreadIcon = Icon(QLatin1String("mail-unread-new"));
	ConfigGroup group = Config().group("contactList");
	d->showOffline = group.value("showOffline", true);
}

TreeModel::~TreeModel()
{
}

QModelIndex TreeModel::index(int row, int, const QModelIndex &parent) const
{
	Q_D(const TreeModel);
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

QModelIndex TreeModel::parent(const QModelIndex &child) const
{
	Q_D(const TreeModel);
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

int TreeModel::rowCount(const QModelIndex &parent) const
{
	Q_D(const TreeModel);
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

int TreeModel::columnCount(const QModelIndex &) const
{
	return 1;
}

bool TreeModel::hasChildren(const QModelIndex &parent) const
{
	Q_D(const TreeModel);
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

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	Q_D(const TreeModel);
	switch(getItemType(index))
	{
	case ContactType:
	{
		ContactItem *item = reinterpret_cast<ContactItem *>(index.internalPointer());
		switch(role)
		{
		case Qt::EditRole:
		case Qt::DisplayRole: {
			QString name = item->data->contact->name();
			return name.isEmpty() ? item->data->contact->id() : name;
		}
		case Qt::DecorationRole:
			if (d->showMessageIcon && d->unreadContacts.contains(item->data->contact))
				return d->unreadIcon;
			else
				return item->data->contact->status().icon();
		case ItemTypeRole:
			return ContactType;
		case StatusRole:
			return qVariantFromValue(item->data->contact->status());
		case AvatarRole:
			return item->data->contact->avatar();
		case BuddyRole: {
			ContactItem *item = reinterpret_cast<ContactItem*>(index.internalPointer());
			Buddy *buddy = item->data->contact;
			return qVariantFromValue(buddy);
		}
		default:
			return QVariant();
		}
	}
	case TagType:
	{
		TagItem *item = reinterpret_cast<TagItem *>(index.internalPointer());
		switch(role)
		{
		case Qt::DisplayRole:
			return item->name;
		case ItemTypeRole:
			return TagType;
		case Qt::DecorationRole:
			return Icon("feed-subscribe");
		case ContactsCountRole:
			return item->contacts.count();
		case OnlineContactsCountRole:
			return item->online;
		default:
			return QVariant();
		}
	}
	default:
		return QVariant();
	}
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_D(const TreeModel);
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section==0) {
		if (d->selectedTags.isEmpty())
			return tr("All tags");
		else
			return tr("Custom tags");
	}

	return QVariant();
}

void TreeModel::addContact(Contact *contact)
{
	Q_D(TreeModel);
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

	if (!meta) {
		meta = static_cast<MetaContact*>(contact->metaContact());
		if (meta && d->contacts.contains(meta))
			return;
		else if (meta)
			contact = meta;
	}

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

	ContactData::Ptr item_data(new ContactData);
	item_data->contact = contact;
	item_data->tags = QSet<QString>::fromList(tags);
	item_data->status = contact->status();
	int counter = item_data->status.type() == Status::Offline ? 0 : 1;
	d->contacts.insert(contact, item_data);
	for(QSet<QString>::const_iterator it = item_data->tags.constBegin(); it != item_data->tags.constEnd(); it++)
	{
		TagItem *tag = ensureTag<TagItem>(d, *it);
		ContactItem *item = new ContactItem(item_data);
		item->parent = tag;
		bool show = isVisible(item);
		tag->online += counter;
		if (show) {
			hideContact<TreeModelPrivate, TagItem>(item, false, false);
		} else {
			tag->contacts.append(item);
			item_data->items.append(item);
		}
	}
}

bool TreeModel::containsContact(Contact *contact) const
{
	return d_func()->contacts.contains(contact);
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::EditRole && getItemType(index) == ContactType) {
		ContactItem *item = reinterpret_cast<ContactItem *>(index.internalPointer());
		item->data->contact->setName(value.toString());
		return true;
	}
	return false;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = QAbstractItemModel::flags(index);

	ContactItemType type = getItemType(index);
	flags |= Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;
	if (type == ContactType)
		flags |= Qt::ItemIsEditable;

	return flags;
}

Qt::DropActions TreeModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

QStringList TreeModel::mimeTypes() const
{
	QStringList types;
	types << QUTIM_MIME_CONTACT_INTERNAL;
	types << QUTIM_MIME_TAG_INTERNAL;
	types << MimeObjectData::objectMimeType();
	return types;
}

QMimeData *TreeModel::mimeData(const QModelIndexList &indexes) const
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

bool TreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
						 int row, int column, const QModelIndex &parent)
{
	Q_D(TreeModel);
	if (action == Qt::IgnoreAction)
		return true;

	ContactItemType parentType = getItemType(parent);
	if (parentType != ContactType && parentType != TagType)
		return false;

	qptrdiff internalId = 0;
	QByteArray encodedData;
	bool isContact = data->hasFormat(QUTIM_MIME_CONTACT_INTERNAL);
	if (isContact)
		encodedData = data->data(QUTIM_MIME_CONTACT_INTERNAL);
	else if (data->hasFormat(QUTIM_MIME_TAG_INTERNAL))
		encodedData = data->data(QUTIM_MIME_TAG_INTERNAL);
	else
		return false;

	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	stream >> row >> column >> internalId;
	QModelIndex index = createIndex(row, column, reinterpret_cast<void*>(internalId));
	if (isContact && getItemType(index) != ContactType)
		return false;

	ChangeEvent *ev = new ChangeEvent;
	ev->child = reinterpret_cast<ItemHelper*>(index.internalPointer());
	ev->parent = reinterpret_cast<ItemHelper*>(parent.internalPointer());
	if (getItemType(index) == TagType)
		ev->type = ChangeEvent::MoveTag;
	else if (getItemType(parent) == TagType)
		ev->type = ChangeEvent::ChangeTags;
	else if(getItemType(parent) == ContactType)
		ev->type = ChangeEvent::MergeContacts;
	d->events << ev;
	d->timer.start(1, this);

	return true;
	// We should return false
	//			return false;
}

void TreeModel::removeFromContactList(Contact *contact, bool deleted)
{
	Q_D(TreeModel);
	Q_UNUSED(deleted);
	ContactData::Ptr item_data = d->contacts.value(contact);
	if(!item_data)
		return;
	int counter = item_data->status.type() == Status::Offline ? 0 : -1;
	for(int i = 0; i < item_data->items.size(); i++) {
		ContactItem *item = item_data->items.at(i);
		item->parent->online += counter;
		hideContact<TreeModelPrivate, TagItem>(item, true, false);
		delete item;
	}
	d->contacts.remove(contact);
	d->unreadContacts.remove(contact);
}

void TreeModel::contactDeleted(QObject *obj)
{
	Contact *contact = reinterpret_cast<Contact *>(obj);
	removeFromContactList(contact,true);
}

void TreeModel::removeContact(Contact *contact)
{
	Q_D(TreeModel);
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
}

void TreeModel::contactStatusChanged(Status status)
{
	Q_D(TreeModel);
	Contact *contact = qobject_cast<Contact *>(sender());
	ContactData::Ptr item_data = d->contacts.value(contact);
	if(!item_data)
		return;
	bool statusTypeChanged = status.type() != item_data->status.type();
	int counter = 0;
	if (status.type() == Status::Offline)
		counter = statusTypeChanged ? -1 : 0;
	else if (item_data->status == Status::Offline)
		counter = +1;
	item_data->status = status;
	const QList<ContactItem *> &items = item_data->items;
	bool show = isVisible(item_data->items.value(0));
	for(int i = 0; i < items.size(); i++) {
		ContactItem *item = items.at(i);
		item->parent->online += counter;

		if (!hideContact<TreeModelPrivate, TagItem>(item, !show)) {
			if (!show)
				// The item is already hidden and it should stay that way.
				return;
		} else {
			// Depending on 'show' the item has been either added to the model
			// or removed from it in hideContact method.
			// Either way, we have nothing to do anymore.
			return;
		}

		// The item is already visible, so we need to move it in the right place
		// and update its content
		updateContact<TreeModelPrivate>(item, statusTypeChanged);
	}
}

void TreeModel::contactNameChanged(const QString &name)
{
	Q_D(TreeModel);
	Q_UNUSED(name);
	Contact *contact = qobject_cast<Contact *>(sender());
	ContactData::Ptr item_data = d->contacts.value(contact);
	if(!item_data)
		return;
	const QList<ContactItem *> &items = item_data->items;
	if (items.isEmpty() || !isVisible(items.first()))
		return;
	for(int i = 0; i < items.size(); i++)
		updateContact<TreeModelPrivate>(items.at(i), true);
}

void TreeModel::onContactInListChanged(bool)
{
	//Contact *contact = qobject_cast<Contact*>(sender());
	//p->contacts.value(contact)->
	//TODO
}

void TreeModel::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),
			this, SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
}

void TreeModel::onUnreadChanged(const qutim_sdk_0_3::MessageList &messages)
{
	Q_D(TreeModel);
	ChatSession *session = qobject_cast<ChatSession*>(sender());
	QSet<Contact*> contacts;
	QSet<ChatUnit*> chatUnits;
	for (int i = 0; i < messages.size(); i++) {
		ChatUnit *unit = messages.at(i).chatUnit();
		if (chatUnits.contains(unit) || !unit)
			continue;
		chatUnits.insert(unit);
		Event event(d->realUnitRequestEvent);
		QCoreApplication::sendEvent(unit, &event);
		Contact *contact = event.at<Contact*>(0);
		while (unit && !contact) {
			if (!!(contact = qobject_cast<Contact*>(unit)))
				break;
			unit = unit->upperUnit();
		}
		if (Contact *meta = qobject_cast<MetaContact*>(contact->metaContact()))
			contact = meta;
		if (contact)
			contacts.insert(contact);
	}
	if (contacts.isEmpty())
		d->unreadBySession.remove(session);
	else
		d->unreadBySession.insert(session, contacts);
	foreach (const QSet<Contact*> &contactsSet, d->unreadBySession)
		contacts |= contactsSet;

	if (!contacts.isEmpty() && !d->unreadTimer.isActive())
		d->unreadTimer.start(500, this);
	else if (contacts.isEmpty())
		d->unreadTimer.stop();

	if (!d->showMessageIcon) {
		d->unreadContacts = contacts;
	} else {
		QSet<Contact*> needUpdate = d->unreadContacts;
		needUpdate.subtract(contacts);
		d->unreadContacts = contacts;
		foreach (Contact *contact, needUpdate) {
			ContactData::Ptr item_data = d->contacts.value(contact);
			for (int i = 0; i < item_data->items.size(); i++) {
				ContactItem *item = item_data->items.at(i);
				QModelIndex index = createIndex(item->index(), 0, item);
				emit dataChanged(index, index);
			}
		}
	}
}

void TreeModel::contactTagsChanged(const QStringList &tags_helper)
{
	Q_D(TreeModel);
	Contact *contact = qobject_cast<Contact *>(sender());
	ContactData::Ptr item_data = d->contacts.value(contact);
	if(!item_data)
		return;
	bool show = isVisible(item_data->items.value(0));
	QSet<QString> tags;
	tags = QSet<QString>::fromList(tags_helper);
	if(tags.isEmpty())
		tags << tr("Without tags");

	//It should be noted in contactlist those who are not in the roster
	//if(!contact->isInList())
	//	tags << tr("Not in list");

	int counter = item_data->status.type() == Status::Offline ? 0 : 1;
	QSet<QString> to_add = tags - item_data->tags;
	for (int i = 0, size = item_data->items.size(); i < size; i++) {
		ContactItem *item = item_data->items.at(i);
		if(tags.contains(item->parent->name))
			continue;
		item->parent->online -= counter;
		hideContact<TreeModelPrivate, TagItem>(item, true, false);
		delete item;
		i--;
		size--;
	}
	for (QSet<QString>::const_iterator it = to_add.constBegin(); it != to_add.constEnd(); it++) {
		TagItem *tag = ensureTag<TagItem>(d, *it);
		tag->online += counter;
		ContactItem *item = new ContactItem(item_data);
		item->parent = tag;
		if (show) {
			hideContact<TreeModelPrivate, TagItem>(item, false, false);
		} else {
			tag->contacts.append(item);
			item_data->items.append(item);
		}
	}
	item_data->tags = tags;
}

void TreeModel::hideShowOffline()
{
	Q_D(TreeModel);
	ConfigGroup group = Config().group("contactList");
	bool show = !group.value("showOffline", true);
	group.setValue("showOffline", show);
	group.sync();
	if (d->showOffline == show)
		return;
	d->showOffline = show;
	filterAllList();
}

void TreeModel::filterList(const QString &filter)
{
	Q_D(TreeModel);
	if (filter == d->lastFilter)
		return;
	d->lastFilter = filter;
	filterAllList();
}

void TreeModel::filterList(const QStringList &tags)
{
	Q_D(TreeModel);
	if (tags == d->selectedTags)
		return;
	d->selectedTags = tags;
	filterAllList();
}

QStringList TreeModel::tags() const
{
	Q_D(const TreeModel);
	QStringList all_tags;
	foreach (const TagItem *tag,d->tags)
		all_tags.append(tag->name);
	return all_tags;
}

QStringList TreeModel::selectedTags() const
{
	return d_func()->selectedTags;
}

void TreeModel::processEvent(ChangeEvent *ev)
{
	Q_D(TreeModel);
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
	} else if (ev->type == ChangeEvent::MergeContacts) { // MetaContacts
		ContactItem *parentItem = reinterpret_cast<ContactItem*>(ev->parent);
		if (item->data->contact == parentItem->data->contact)
			return;
		MetaContact *childMeta = qobject_cast<MetaContact*>(item->data->contact);
		MetaContact *meta = qobject_cast<MetaContact*>(parentItem->data->contact);

		QString text;
		if (!childMeta && !meta) {
			text = tr("Would you like to merge contacts \"%1\" <%2> and \"%3\" <%4>?");
			text = text.arg(item->data->contact->name(),
							item->data->contact->id(),
							parentItem->data->contact->name(),
							parentItem->data->contact->id());
		} else if (childMeta && meta) {
			text = tr("Would you like to merge metacontacts \"%1\" and \"%2\"?");
			text = text.arg(childMeta->title(), meta->title());
		} else {
			text = tr("Would you like to add \"%1\" <%2> to metacontact \"%3\"?");
			Contact *c = (meta ? item : parentItem)->data->contact;
			MetaContact *m = meta ? meta : childMeta;
			text = text.arg(c->name(), c->id(), m->name());
		}

		if (QMessageBox::Yes == QMessageBox::question(qobject_cast<QWidget*>(QObject::parent()),
													  tr("Contacts' merging"), text,
													  QMessageBox::Yes | QMessageBox::No)) {
			if (childMeta && !meta) {
				meta = childMeta;
				childMeta = 0;
			} else if (!meta) {
				meta = MetaContactManager::instance()->createContact();
				meta->addContact(parentItem->data->contact);
			} else if (childMeta && meta) {
				foreach (ChatUnit *unit, childMeta->lowerUnits()) {
					Contact *contact = qobject_cast<Contact*>(unit);
					if (contact)
						meta->addContact(contact);
				}
				childMeta->deleteLater();
				return;
			}
			if (!childMeta)
				meta->addContact(item->data->contact);
		}
	} else if (ev->type == ChangeEvent::MoveTag) {
		int to = -2, globalTo = -2;
		if (ev->parent->type == ContactType) {
			TagItem *tag = reinterpret_cast<ContactItem*>(ev->parent)->parent;
			to = d->visibleTags.indexOf(tag) + 1;
			globalTo = d->tags.indexOf(tag) + 1;
		} else if (ev->parent->type == TagType) {
			TagItem *tag = reinterpret_cast<TagItem*>(ev->parent);
			to = d->visibleTags.indexOf(tag);
			globalTo = d->tags.indexOf(tag);
		} else {
			Q_ASSERT(!"Not implemented");
		}
		TagItem *tag = reinterpret_cast<TagItem*>(ev->child);
		int from = d->visibleTags.indexOf(tag);
		int globalFrom = d->tags.indexOf(tag);
		Q_ASSERT(from >= 0 && to >= 0 && globalTo >= 0 && globalFrom >= 0);
		if (beginMoveRows(QModelIndex(), from, from, QModelIndex(), to)) {
			if (from < to) {
				Q_ASSERT(globalFrom < globalTo);
				--to;
				--globalTo;
			}
			d->visibleTags.move(from, to);
			d->tags.move(globalFrom, globalTo);
			endMoveRows();
		}
		saveTagOrder();
	}
}

void TreeModel::timerEvent(QTimerEvent *timerEvent)
{
	Q_D(TreeModel);
	if (timerEvent->timerId() == d->timer.timerId()) {
		for (int i = 0; i < d->events.size(); i++) {
			processEvent(d->events.at(i));
			delete d->events.at(i);
		}
		d->events.clear();
		d->timer.stop();
		return;
	} else if (timerEvent->timerId() == d->unreadTimer.timerId()) {
		foreach (Contact *contact, d->unreadContacts) {
			ContactData::Ptr item_data = d->contacts.value(contact);
			//if (!item_data) {//FIXME why p->contacts doesn't contains contact
			//	qDebug() << "Unread" << contact <<  p->unreadContacts;
			//	continue;
			//}
			for (int i = 0; i < item_data->items.size(); i++) {
				ContactItem *item = item_data->items.at(i);
				QModelIndex index = createIndex(item->index(), 0, item);
				emit dataChanged(index, index);
			}
		}
		d->showMessageIcon = !d->showMessageIcon;
		return;
	}
	QAbstractItemModel::timerEvent(timerEvent);
}

bool TreeModel::eventFilter(QObject *obj, QEvent *ev)
{
	Q_D(TreeModel);
	if (ev->type() == MetaContactChangeEvent::eventType()) {
		MetaContactChangeEvent *metaEvent = static_cast<MetaContactChangeEvent*>(ev);
		if (metaEvent->oldMetaContact() && !metaEvent->newMetaContact())
			addContact(metaEvent->contact());
		else if (!metaEvent->oldMetaContact() && metaEvent->newMetaContact())
			removeContact(metaEvent->contact());
		return false;
	} else if (ev->type() == Event::eventType()) {
		Event *event = static_cast<Event*>(ev);
		if (d->initData && event->id == d->initData->qutimStartupEvent)
			initialize();
		return false;
	}
	return QAbstractItemModel::eventFilter(obj, ev);
}

void TreeModel::filterAllList()
{
	Q_D(TreeModel);
	for (int i = 0; i < d->tags.size(); i++) {
		TagItem *tag = d->tags.at(i);
		bool tagFiltered = !d->selectedTags.isEmpty() && !d->selectedTags.contains(tag->name);
		foreach (ContactItem *item, tag->contacts)
			hideContact<TreeModelPrivate, TagItem>(item, tagFiltered || !isVisible(item));
	}
}

bool TreeModel::isVisible(ContactItem *item)
{
	Q_D(TreeModel);
	if (!item) {
		qWarning() << Q_FUNC_INFO << "item is null";
		return true;
	}
	Contact *contact = item->data->contact;
	if (!d->lastFilter.isEmpty()) {
		return contact->id().contains(d->lastFilter,Qt::CaseInsensitive)
				|| contact->name().contains(d->lastFilter,Qt::CaseInsensitive);
	} else if (!d->selectedTags.isEmpty() && !d->selectedTags.contains(item->parent->name)) {
		return false;
	} else {
		return d->showOffline || contact->status().type() != Status::Offline;
	}
}

void TreeModel::initialize()
{
	Q_D(TreeModel);
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	connect(MetaContactManager::instance(), SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			this, SLOT(addContact(qutim_sdk_0_3::Contact*)));

	foreach(Protocol *proto, Protocol::all()) {
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach(Account *account, proto->accounts())
			onAccountCreated(account);
	}

	TreeModelPrivate::InitData *initData = d->initData;
	d->initData = 0;
	// ensure correct order of tags
	QSet<QString> tags;
	foreach (Contact *contact, initData->contacts)
		foreach (const QString &tag, contact->tags())
			tags.insert(tag);
	foreach (const QString &tag, Config().value("contactList/tags", QStringList()))
		if (tags.contains(tag))
			ensureTag<TagItem>(d, tag);
	// add contacts to the contact list
	foreach (Contact *contact, initData->contacts)
		addContact(contact);
	delete initData;
}

void TreeModel::saveTagOrder()
{
	Q_D(TreeModel);
	Config group = Config().group("contactList");
	QStringList tags;
	foreach (TagItem *tag, d->tags)
		tags << tag->name;
	group.setValue("tags", tags);
}

bool TreeModel::showOffline() const
{
	return d_func()->showOffline;
}

void TreeModel::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	foreach (Contact *contact, account->findChildren<Contact*>())
		addContact(contact);
	connect(account, SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			this, SLOT(addContact(qutim_sdk_0_3::Contact*)));
}

}
}


