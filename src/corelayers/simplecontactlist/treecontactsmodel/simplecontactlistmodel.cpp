#include "simplecontactlistmodel.h"
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

struct ModelPrivate
{
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
	quint16 qutimAboutToQuitEvent;

	struct InitData
	{
		QList<Contact*> contacts;
		quint16 qutimStartupEvent;
	};
	// Pointer to variables that are solely used at startup.
	// See Model::initialize()
	InitData *initData;
};

Model::Model(QObject *parent) : AbstractContactModel(parent), p(new ModelPrivate)
{
	p->showMessageIcon = false;
	Event::eventManager()->installEventFilter(this);
	p->initData = new ModelPrivate::InitData;
	p->realUnitRequestEvent = Event::registerType("real-chatunit-request");
	p->qutimAboutToQuitEvent = Event::registerType("aboutToQuit");
	p->initData->qutimStartupEvent = Event::registerType("startup");
	p->unreadIcon = Icon(QLatin1String("mail-unread-new"));
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	ConfigGroup group = Config().group("contactList");
	p->showOffline = group.value("showOffline", true);
}

Model::~Model()
{
}

QModelIndex Model::index(int row, int, const QModelIndex &parent) const
{
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
		if(p->visibleTags.size() <= row)
			return QModelIndex();
		return createIndex(row, 0, p->visibleTags.at(row));
	}
}

QModelIndex Model::parent(const QModelIndex &child) const
{
	switch(getItemType(child))
	{
	case ContactType: {
		ContactItem *item = reinterpret_cast<ContactItem *>(child.internalPointer());
		return createIndex(p->visibleTags.indexOf(item->parent), 0, item->parent);
	}
	case TagType:
	default:
		return QModelIndex();
	}
}

int Model::rowCount(const QModelIndex &parent) const
{
	switch(getItemType(parent))
	{
	case TagType:
		return reinterpret_cast<TagItem *>(parent.internalPointer())->visible.size();
	case ContactType:
		return 0;
	default:
		return p->visibleTags.size();
	}
}

int Model::columnCount(const QModelIndex &) const
{
	return 1;
}

bool Model::hasChildren(const QModelIndex &parent) const
{
	switch(getItemType(parent))
	{
	case TagType:
		return !reinterpret_cast<TagItem *>(parent.internalPointer())->visible.isEmpty();
	case ContactType:
		return false;
	default:
		return !p->visibleTags.isEmpty();
	}
}

QVariant Model::data(const QModelIndex &index, int role) const
{
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
			if (p->showMessageIcon && p->unreadContacts.contains(item->data->contact))
				return p->unreadIcon;
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

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section==0) {
		if (p->selectedTags.isEmpty())
			return tr("All tags");
		else
			return tr("Custom tags");
	}

	return QVariant();
}

bool contactLessThan (ContactItem *a, ContactItem *b) {
	int result;

	//int unreadA = 0;
	//int unreadB = 0;
	//ChatSession *session = ChatLayer::get(a->data->contact,false);
	//if(session)
	//	unreadA = session->unread().count();
	//session = ChatLayer::get(b->data->contact,false);
	//if(session)
	//	unreadB = session->unread().count();
	//result = unreadA - unreadB;
	//if(result)
	//	return result < 0;

	result = a->data->status.type() - b->data->status.type();
	if (result)
		return result < 0;
	return a->data->contact->title().compare(b->data->contact->title(), Qt::CaseInsensitive) < 0;
};

void Model::addContact(Contact *contact)
{
	//TODO implement more powerfull logic
	//			if (!contact->isInList())
	//				return;

	if (p->initData) {
		if (p->initData->contacts.contains(contact))
			return;
		p->initData->contacts << contact;
		return;
	}

	if (p->contacts.contains(contact))
		return;

	MetaContact *meta = qobject_cast<MetaContact*>(contact);

	if (!meta) {
		meta = static_cast<MetaContact*>(contact->metaContact());
		if (meta && p->contacts.contains(meta))
			return;
		else if (meta)
			contact = meta;
	}

	if (meta) {
		meta->installEventFilter(this);
		foreach (ChatUnit *unit, meta->lowerUnits()) {
			Contact *subContact = qobject_cast<Contact*>(unit);
			if (subContact && p->contacts.contains(subContact))
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
		tags << QLatin1String("Default");

	ContactData::Ptr item_data(new ContactData);
	item_data->contact = contact;
	item_data->tags = QSet<QString>::fromList(tags);
	item_data->status = contact->status();
	int counter = item_data->status.type() == Status::Offline ? 0 : 1;
	p->contacts.insert(contact, item_data);
	for(QSet<QString>::const_iterator it = item_data->tags.constBegin(); it != item_data->tags.constEnd(); it++)
	{
		TagItem *tag = ensureTag(*it);
		ContactItem *item = new ContactItem(item_data);
		item->parent = tag;
		bool show = isVisible(item);
		tag->online += counter;
		if (show) {
			hideContact(item, false, false);
		} else {
			tag->contacts.append(item);
			item_data->items.append(item);
		}
	}
}

bool Model::containsContact(Contact *contact) const
{
	return p->contacts.contains(contact);
}

bool Model::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::EditRole && getItemType(index) == ContactType) {
		ContactItem *item = reinterpret_cast<ContactItem *>(index.internalPointer());
		item->data->contact->setName(value.toString());
		return true;
	}
	return false;
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = QAbstractItemModel::flags(index);

	ContactItemType type = getItemType(index);
	flags |= Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;
	if (type == ContactType)
		flags |= Qt::ItemIsEditable;

	return flags;
}

Qt::DropActions Model::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

QStringList Model::mimeTypes() const
{
	QStringList types;
	types << QUTIM_MIME_CONTACT_INTERNAL;
	types << QUTIM_MIME_TAG_INTERNAL;
	types << MimeObjectData::objectMimeType();
	return types;
}

QMimeData *Model::mimeData(const QModelIndexList &indexes) const
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

bool Model::dropMimeData(const QMimeData *data, Qt::DropAction action,
						 int row, int column, const QModelIndex &parent)
{
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
	p->events << ev;
	p->timer.start(1, this);

	return true;
	// We should return false
	//			return false;
}

void Model::removeFromContactList(Contact *contact, bool deleted)
{
	Q_UNUSED(deleted);
	ContactData::Ptr item_data = p->contacts.value(contact);
	if(!item_data)
		return;
	int counter = item_data->status.type() == Status::Offline ? 0 : -1;
	for(int i = 0; i < item_data->items.size(); i++) {
		ContactItem *item = item_data->items.at(i);
		item->parent->online += counter;
		hideContact(item, true, false);
		delete item;
	}
	p->contacts.remove(contact);
}

void Model::contactDeleted(QObject *obj)
{
	Contact *contact = reinterpret_cast<Contact *>(obj);
	removeFromContactList(contact,true);
}

void Model::removeContact(Contact *contact)
{
	Q_ASSERT(contact);
	if (MetaContact *meta = qobject_cast<MetaContact*>(contact)) {
		contact->removeEventFilter(this);
		foreach (ChatUnit *unit, meta->lowerUnits()) {
			Contact *subContact = qobject_cast<Contact*>(unit);
			if (subContact && !p->contacts.contains(subContact))
				addContact(subContact);
		}
	}
	contact->disconnect(this);
	removeFromContactList(contact,false);
}

void Model::contactStatusChanged(Status status)
{
	Contact *contact = qobject_cast<Contact *>(sender());
	ContactData::Ptr item_data = p->contacts.value(contact);
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

		if (!hideContact(item, !show)) {
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
		updateContact(item, statusTypeChanged);
	}
}

void Model::contactNameChanged(const QString &name)
{
	Q_UNUSED(name);
	Contact *contact = qobject_cast<Contact *>(sender());
	ContactData::Ptr item_data = p->contacts.value(contact);
	if(!item_data)
		return;
	const QList<ContactItem *> &items = item_data->items;
	if (items.isEmpty() || !isVisible(items.first()))
		return;
	for(int i = 0; i < items.size(); i++)
		updateContact(items.at(i), true);
}

void Model::onContactInListChanged(bool)
{
	//Contact *contact = qobject_cast<Contact*>(sender());
	//p->contacts.value(contact)->
	//TODO
}

void Model::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),
			this, SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
}

void Model::onUnreadChanged(const qutim_sdk_0_3::MessageList &messages)
{
	ChatSession *session = qobject_cast<ChatSession*>(sender());
	QSet<Contact*> contacts;
	QSet<ChatUnit*> chatUnits;
	for (int i = 0; i < messages.size(); i++) {
		ChatUnit *unit = const_cast<ChatUnit*>(messages.at(i).chatUnit());
		if (chatUnits.contains(unit) || !unit)
			continue;
		chatUnits.insert(unit);
		Event event(p->realUnitRequestEvent);
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
		p->unreadBySession.remove(session);
	else
		p->unreadBySession.insert(session, contacts);
	foreach (const QSet<Contact*> &contactsSet, p->unreadBySession)
		contacts |= contactsSet;

	if (!contacts.isEmpty() && !p->unreadTimer.isActive())
		p->unreadTimer.start(500, this);
	else if (contacts.isEmpty())
		p->unreadTimer.stop();

	if (!p->showMessageIcon) {
		p->unreadContacts = contacts;
	} else {
		QSet<Contact*> needUpdate = p->unreadContacts;
		needUpdate.subtract(contacts);
		p->unreadContacts = contacts;
		foreach (Contact *contact, needUpdate) {
			ContactData::Ptr item_data = p->contacts.value(contact);
			for (int i = 0; i < item_data->items.size(); i++) {
				ContactItem *item = item_data->items.at(i);
				QModelIndex index = createIndex(item->index(), 0, item);
				emit dataChanged(index, index);
			}
		}
	}
}

void Model::contactTagsChanged(const QStringList &tags_helper)
{
	Contact *contact = qobject_cast<Contact *>(sender());
	ContactData::Ptr item_data = p->contacts.value(contact);
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
		hideContact(item, true, false);
		delete item;
		i--;
		size--;
	}
	for (QSet<QString>::const_iterator it = to_add.constBegin(); it != to_add.constEnd(); it++) {
		TagItem *tag = ensureTag(*it);
		tag->online += counter;
		ContactItem *item = new ContactItem(item_data);
		item->parent = tag;
		if (show) {
			hideContact(item, false, false);
		} else {
			tag->contacts.append(item);
			item_data->items.append(item);
		}
	}
	item_data->tags = tags;
}

void Model::hideShowOffline()
{
	ConfigGroup group = Config().group("contactList");
	bool show = !group.value("showOffline", true);
	group.setValue("showOffline", show);
	group.sync();
	if (p->showOffline == show)
		return;
	p->showOffline = show;
	filterAllList();
}

void Model::filterList(const QString &filter)
{
	if (filter == p->lastFilter)
		return;
	p->lastFilter = filter;
	filterAllList();
}

void Model::filterList(const QStringList &tags)
{
	if (tags == p->selectedTags)
		return;
	p->selectedTags = tags;
	filterAllList();
}

QStringList Model::tags() const
{
	QStringList all_tags;
	foreach (const TagItem *tag,p->tags)
		all_tags.append(tag->name);
	return all_tags;
}

QStringList Model::selectedTags() const
{
	return p->selectedTags;
}

void Model::processEvent(ChangeEvent *ev)
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
	}
}

void Model::timerEvent(QTimerEvent *timerEvent)
{
	if (timerEvent->timerId() == p->timer.timerId()) {
		for (int i = 0; i < p->events.size(); i++) {
			processEvent(p->events.at(i));
			delete p->events.at(i);
		}
		p->events.clear();
		p->timer.stop();
		return;
	} else if (timerEvent->timerId() == p->unreadTimer.timerId()) {
		foreach (Contact *contact, p->unreadContacts) {
			ContactData::Ptr item_data = p->contacts.value(contact);
			for (int i = 0; i < item_data->items.size(); i++) {
				ContactItem *item = item_data->items.at(i);
				QModelIndex index = createIndex(item->index(), 0, item);
				emit dataChanged(index, index);
			}
		}
		p->showMessageIcon = !p->showMessageIcon;
		return;
	}
	QAbstractItemModel::timerEvent(timerEvent);
}

bool Model::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == MetaContactChangeEvent::eventType()) {
		MetaContactChangeEvent *metaEvent = static_cast<MetaContactChangeEvent*>(ev);
		if (metaEvent->oldMetaContact() && !metaEvent->newMetaContact())
			addContact(metaEvent->contact());
		else if (!metaEvent->oldMetaContact() && metaEvent->newMetaContact())
			removeContact(metaEvent->contact());
		return false;
	} else if (ev->type() == Event::eventType()) {
		Event *event = static_cast<Event*>(ev);
		if (p->initData && event->id == p->initData->qutimStartupEvent)
			initialize();
		else if (event->id == p->qutimAboutToQuitEvent)
			saveConfig();
		return false;
	}
	return QAbstractItemModel::eventFilter(obj, ev);
}

bool Model::event(QEvent *ev)
{
	if (ev->type() == ActionVisibilityChangedEvent::eventType()) {
		ActionVisibilityChangedEvent *event = static_cast<ActionVisibilityChangedEvent*>(ev);
		if (event->isVisible()) {
			QAction *action = event->action();
			Contact *contact = qobject_cast<Contact*>(event->controller());
			if (!contact)
				return false;
			if (contact->isInList()) {
				action->setIcon(Icon("list-remove"));
				action->setText(QT_TRANSLATE_NOOP("ContactList", "Remove contact"));
			} else {
				action->setIcon(Icon("list-add"));
				action->setText(QT_TRANSLATE_NOOP("ContactList", "Add to contact list"));
			}
			action->setEnabled(contact->account()->status() != qutim_sdk_0_3::Status::Offline); //experiment
			return true;
		}
	}
	return QObject::event(ev);
}

void Model::filterAllList()
{
	for (int i = 0; i < p->tags.size(); i++) {
		TagItem *tag = p->tags.at(i);
		bool tagFiltered = !p->selectedTags.isEmpty() && !p->selectedTags.contains(tag->name);
		foreach (ContactItem *item, tag->contacts)
			hideContact(item, tagFiltered || !isVisible(item));
	}
}

bool Model::isVisible(ContactItem *item)
{
	if (!item) {
		qWarning() << Q_FUNC_INFO << "item is null";
		return true;
	}
	Contact *contact = item->data->contact;
	if (!p->lastFilter.isEmpty()) {
		return contact->id().contains(p->lastFilter,Qt::CaseInsensitive)
				|| contact->name().contains(p->lastFilter,Qt::CaseInsensitive);
	} else if (!p->selectedTags.isEmpty() && !p->selectedTags.contains(item->parent->name)) {
		return false;
	} else {
		return p->showOffline || contact->status().type() != Status::Offline;
	}
}

bool Model::hideContact(ContactItem *item, bool hide, bool replacing)
{
	TagItem *tag = item->parent;
	Q_ASSERT(tag);
	Q_ASSERT(!replacing || tag->contacts.contains(item));
	if (!hide)
		showTag(tag);
	int row = p->visibleTags.indexOf(tag);
	QModelIndex tagIndex = createIndex(row, 0, tag);
	if (hide) {
		int index = tag->visible.indexOf(item);
		if (row == -1 || index == -1) {
			if (!replacing) {
				item->parent->contacts.removeOne(item);
				item->data->items.removeOne(item);
			}
			return false;
		}
		beginRemoveRows(tagIndex, index, index);
		tag->visible.removeAt(index);
		if (!replacing) {
			item->parent->contacts.removeOne(item);
			item->data->items.removeOne(item);
		}
		endRemoveRows();
		if (tag->visible.isEmpty())
			hideTag(tag);
	} else {
		Q_ASSERT(row >= 0);
		Q_ASSERT(isVisible(item));
		if (tag->visible.contains(item))
			return false;
		QList<ContactItem *> &contacts = tag->visible;
		QList<ContactItem *>::const_iterator contacts_it =
				qLowerBound(contacts.constBegin(), contacts.constEnd(), item, contactLessThan);
		int index = contacts_it - contacts.constBegin();

		beginInsertRows(tagIndex, index, index);
		if (!replacing) {
			item->parent->contacts.append(item);
			item->data->items.append(item);
		}
		contacts.insert(index, item);
		endInsertRows();
	}
	return true;
}

void Model::hideTag(TagItem *item)
{
	Q_ASSERT(p->tags.contains(item));
	int index = p->visibleTags.indexOf(item);
	if (index == -1)
		return; // The tag is already hidden
	beginRemoveRows(QModelIndex(), index, index);
	p->visibleTags.removeAt(index);
	endRemoveRows();
	emit tagVisibilityChanged(createIndex(index, 0, item), item->name, false);
	if (item->contacts.isEmpty()) {
		p->tagsHash.remove(item->name);
		p->tags.removeOne(item);
		delete item;
	}
}

void Model::showTag(TagItem *item)
{
	Q_ASSERT(p->tags.contains(item));
	int index = p->visibleTags.indexOf(item);
	if (index >= 0)
		return; // The tag is already in the contact list
	// A wierd way to find out tag's index, but
	// we need it to ensure the right order of tags.
	index = 0;
	for (int j = 0, tc = p->tags.count(), vc = p->visibleTags.count(); j < tc && index != vc; ++j) {
		TagItem *currentTag = p->tags.at(j);
		if (currentTag == item)
			break; // The index is found.
		if (currentTag == p->visibleTags.at(index))
			// We found a visible tag that is before our tag,
			// thus increase the index.
			++index;
	}
	// We can add the tag to the contact list now
	beginInsertRows(QModelIndex(), index, index);
	p->visibleTags.insert(index, item);
	endInsertRows();
	emit tagVisibilityChanged(createIndex(index, 0, item), item->name, true);
}

TagItem *Model::ensureTag(const QString &name)
{
	TagItem *tag = 0;
	if(!(tag = p->tagsHash.value(name, 0))) {
		tag = new TagItem;
		tag->name = name;
		p->tagsHash.insert(tag->name, tag);
		p->tags << tag;

	}
	return tag;
}

void Model::updateContact(ContactItem *item, bool placeChanged)
{
	QList<ContactItem *> &contacts = item->parent->visible;
	int from = contacts.indexOf(item);
	int to;

	if (from == -1)
		return; // Don't try to move or update a hidden contact

	if (placeChanged) {
		QList<ContactItem *>::const_iterator it =
				qLowerBound(contacts.constBegin(), contacts.constEnd(), item, contactLessThan);
		to = it - contacts.constBegin();
	} else {
		to = from;
	}

	QModelIndex parentIndex = createIndex(p->visibleTags.indexOf(item->parent), 0, item->parent);

	if (from == to) {
		QModelIndex index = createIndex(item->index(), 0, item);
		emit dataChanged(index, index);
	} else {
		if (to == -1 || to > contacts.count())
			return;

		if (beginMoveRows(parentIndex, from, from, parentIndex, to)) {
			if (from < to)
				--to;
			contacts.move(from,to);
			//item_data->items.move(from,to); //FIXME
			endMoveRows();
		}
	}
}

void Model::initialize()
{
	ModelPrivate::InitData *initData = p->initData;
	p->initData = 0;
	Config cfg = Config().group("contactList");
	// ensure correct order of tags
	QSet<QString> tags;
	foreach (Contact *contact, initData->contacts)
		foreach (const QString &tag, contact->tags())
			tags.insert(tag);
	foreach (const QString &tag, cfg.value("tags", QStringList()))
		if (tags.contains(tag))
			ensureTag(tag);
	// add contacts to the contact list
	foreach (Contact *contact, initData->contacts)
		addContact(contact);
	delete initData;
}

void Model::saveConfig()
{
	Config group = Config().group("contactList");
	QStringList tags;
	foreach (TagItem *tag, p->tags)
		tags << tag->name;
	group.setValue("tags", tags);
}

bool Model::showOffline() const
{
	return p->showOffline;
}

}
}
