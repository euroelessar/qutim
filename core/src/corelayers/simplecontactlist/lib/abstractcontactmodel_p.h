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
#ifndef ABSTRACTCONTACTMODEL_P_H
#define ABSTRACTCONTACTMODEL_P_H

#include "abstractcontactmodel.h"
#include "simplecontactlistitem.h"
#include <qutim/metacontactmanager.h>
#include <qutim/metacontact.h>
#include <QBasicTimer>
#include <qutim/icon.h>
#include "qlist.h"
#include <QMessageBox>
#include <qutim/servicemanager.h>

namespace qutim_sdk_0_3
{
	class ChatSession;
}

#define QUTIM_MIME_CONTACT_INTERNAL QLatin1String("application/qutim-contact-internal")
#define QUTIM_MIME_TAG_INTERNAL QLatin1String("application/qutim-tag-internal")

namespace Core {
namespace SimpleContactList {

Contact *getRealUnit(QObject *obj);

class ChangeEvent
{
public:
	enum Type { ChangeTags, MergeContacts, MoveTag } type;
	void *child;
	ItemHelper *parent;
};

class SIMPLECONTACTLIST_EXPORT NotificationsQueue
{
public:
	void append(Notification *notification);
	bool remove(Notification *notification);
	Notification *first() const;
	bool isEmpty();
	QList<QList<Notification*> > all();
private:
	QList<Notification*> m_messageNotifications;
	QList<Notification*> m_typingNotifications;
	QList<Notification*> m_notifications;
};

class AbstractContactModelPrivate
{
public:
	QSet<QString> selectedTags;
	QString lastFilter;
	QList<ChangeEvent*> events;
	QBasicTimer timer;
	QBasicTimer notificationTimer;
	QHash<Contact*, NotificationsQueue> notifications;
	QIcon mailIcon;
	QIcon typingIcon;
	QIcon chatUserJoinedIcon;
	QIcon chatUserLeftIcon;
	QIcon qutimIcon;
	QIcon transferCompletedIcon;
	QIcon birthdayIcon;
	QIcon defaultNotificationIcon;
	bool showNotificationIcon;
	bool showOffline;
};

static ServicePointer<ContactComparator> contactComparator;

template<typename ContactItem>
bool contactLessThan(ContactItem *a, ContactItem *b) {
	Contact * const aContact = a->getContact();
	Contact * const bContact = b->getContact();
	if (!bContact || !aContact || contactComparator.isNull())
		return false;
	return contactComparator.data()->compare(aContact, bContact) < 0;
}

template<typename TagContainer, typename TagItem, typename ContactItem>
bool AbstractContactModel::hideContact(ContactItem *item, bool hide, bool replacing)
{
	TagItem *tag = item->parent;
	Q_ASSERT(tag);
	Q_ASSERT(!replacing || tag->contacts.contains(item));
	if (!hide)
		showTag<TagContainer, TagItem>(tag);
	QModelIndex tagIndex = item->parentIndex(this);;
	int row = tagIndex.row();
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
			hideTag<TagContainer, TagItem>(tag);
	} else {
		Q_ASSERT(row >= 0);
		Q_ASSERT(isVisible(item));
		if (tag->visible.contains(item))
			return false;
		typedef QList<ContactItem *> ContactList;
		ContactList &contacts = tag->visible;
		typename ContactList::const_iterator contacts_it =
				qLowerBound(contacts.constBegin(), contacts.constEnd(), item, contactLessThan<ContactItem>);
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

template<typename TagContainer, typename TagItem>
void AbstractContactModel::hideTag(TagItem *item)
{
	TagContainer *p = item->getTagContainer(this);
	Q_ASSERT(p->tags.contains(item));
	int index = p->visibleTags.indexOf(item);
	if (index == -1)
		return; // The tag is already hidden
	beginRemoveRows(item->parentIndex(this), index, index);
	p->visibleTags.removeAt(index);
	endRemoveRows();
	if (item->contacts.isEmpty()) {
		p->tagsHash.remove(item->name);
		p->tags.removeOne(item);
		delete item;
	}
}

template<typename TagContainer, typename TagItem>
void AbstractContactModel::showTag(TagItem *item)
{
	TagContainer *p = item->getTagContainer(this);
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
	beginInsertRows(item->parentIndex(this), index, index);
	p->visibleTags.insert(index, item);
	endInsertRows();
}

template<typename TagItem, typename TagContainer>
TagItem *AbstractContactModel::ensureTag(TagContainer *p, const QString &name)
{
	Q_ASSERT(p);
	TagItem *tag = 0;
	if(!(tag = p->tagsHash.value(name, 0))) {
		tag = new TagItem;
		tag->name = name;
		tag->setTagContainer(p);
		p->tagsHash.insert(tag->name, tag);
		p->tags << tag;
	}
	return tag;
}

template<typename ContactItem>
void AbstractContactModel::updateContact(ContactItem *item, bool placeChanged)
{
	QList<ContactItem *> &contacts = item->siblings(this);
	int from = contacts.indexOf(item);
	int to;

	if (from == -1)
		return; // Don't try to move or update a hidden contact

	if (placeChanged) {
		typedef QList<ContactItem *> ContactList;
		typename ContactList::const_iterator it =
				qLowerBound(contacts.constBegin(), contacts.constEnd(), item, contactLessThan<ContactItem>);
		to = it - contacts.constBegin();
	} else {
		to = from;
	}

	QModelIndex parentIndex = item->parentIndex(this);

	if (from == to) {
		QModelIndex index = createIndex(item->siblings(this).indexOf(item), 0, item);
		emit dataChanged(index, index);
	} else {
		if (to == -1 || to > contacts.count())
			return;

		if (beginMoveRows(parentIndex, from, from, parentIndex, to)) {
			if (from < to)
				--to;
			contacts.move(from,to);
			endMoveRows();
		}
	}
}

template<typename ContactItem>
QVariant AbstractContactModel::contactData(const QModelIndex &index, int role) const
{
	Q_D(const AbstractContactModel);
	ContactItem *item = reinterpret_cast<ContactItem *>(index.internalPointer());
	Contact *contact = item->getContact();
	if (!contact)
		return QVariant();
	switch(role)
	{
	case Qt::EditRole:
	case Qt::DisplayRole: {
		QString name = contact->name();
		return name.isEmpty() ? contact->id() : name;
	}
	case Qt::DecorationRole:
		if (d->showNotificationIcon) {
			Notification *notif = d->notifications.value(contact).first();
			if (notif)
				return getIconForNotification(notif);
		}
		return contact->status().icon();
	case ItemTypeRole:
		return ContactType;
	case StatusRole:
		return qVariantFromValue(contact->status());
	case AvatarRole:
		return contact->avatar();
	case BuddyRole: {
		return qVariantFromValue<Buddy*>(contact);
	}
	default:
		return QVariant();
	}
}

template<typename TagItem>
QVariant AbstractContactModel::tagData(const QModelIndex &index, int role) const
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
	case TagName:
		return item->getName();
	default:
		return QVariant();
	}
}

template<typename AccountItem>
QVariant AbstractContactModel::accountData(const QModelIndex &index, int role) const
{
	AccountItem *item = reinterpret_cast<AccountItem *>(index.internalPointer());
	switch(role)
	{
	case Qt::DisplayRole:
		return item->account->name() +
				QLatin1String(" (") +
				item->account->id() +
				QLatin1String(")");
	case ItemTypeRole:
		return AccountType;
	case Qt::DecorationRole:
		return item->account->status().icon();
	case AccountRole:
		return qVariantFromValue(item->account);
	case TagName:
		return item->id;
	default:
		return QVariant();
	}
}

template<typename TagContainer, typename TagItem, typename ContactData, typename ContactItem>
void AbstractContactModel::updateContactStatus(typename ContactData::Ptr item_data, const Status &status)
{
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

		if (!hideContact<TagContainer, TagItem>(item, !show)) {
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

template<typename TagContainer, typename TagItem, typename ContactData, typename ContactItem>
void AbstractContactModel::updateContactTags(TagContainer *p, typename ContactData::Ptr item_data, const QStringList &tags_helper)
{
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
		hideContact<TagContainer, TagItem>(item, true, false);
		delete item;
		i--;
		size--;
	}
	for (QSet<QString>::const_iterator it = to_add.constBegin(); it != to_add.constEnd(); it++) {
		TagItem *tag = ensureTag<TagItem>(p, *it);
		tag->online += counter;
		ContactItem *item = new ContactItem(item_data);
		item->parent = tag;
		if (show) {
			hideContact<TagContainer, TagItem>(item, false, false);
		} else {
			tag->contacts.append(item);
			item_data->items.append(item);
		}
	}
	item_data->tags = tags;
}

template<typename ContactItem>
bool AbstractContactModel::isVisible(ContactItem *item)
{
	Q_D(AbstractContactModel);
	if (!item) {
		warning() << Q_FUNC_INFO << "item is null";
		return true;
	}
	Contact *contact = item->getContact();
	if (!contact)
		return false;
	if (!d->lastFilter.isEmpty()) {
		return contact->id().contains(d->lastFilter,Qt::CaseInsensitive)
				|| contact->name().contains(d->lastFilter,Qt::CaseInsensitive);
	} else if (!d->selectedTags.isEmpty() && !item->isInSelectedTag(d->selectedTags)) {
		return false;
	} else {
		return d->showOffline || item->getStatus().type() != Status::Offline;
	}
}

template<typename TagContainer, typename TagItem, typename ContactItem>
void AbstractContactModel::moveTag(ChangeEvent *ev)
{
	TagContainer *p;
	int to = -2, globalTo = -2;
	if (ev->parent->type == ContactType) {
		TagItem *tag = reinterpret_cast<ContactItem*>(ev->parent)->parent;
		p = tag->getTagContainer(this);
		to = p->visibleTags.indexOf(tag) + 1;
		globalTo = p->tags.indexOf(tag) + 1;
	} else if (ev->parent->type == TagType) {
		TagItem *tag = reinterpret_cast<TagItem*>(ev->parent);
		p = tag->getTagContainer(this);
		to = p->visibleTags.indexOf(tag);
		globalTo = p->tags.indexOf(tag);
	} else {
		Q_ASSERT(!"Not implemented");
	}
	TagItem *tag = reinterpret_cast<TagItem*>(ev->child);
	Q_ASSERT(p == tag->getTagContainer(this));
	int from = p->visibleTags.indexOf(tag);
	int globalFrom = p->tags.indexOf(tag);
	Q_ASSERT(from >= 0 && to >= 0 && globalTo >= 0 && globalFrom >= 0);
	QModelIndex parentIndex = tag->parentIndex(this);
	if (beginMoveRows(parentIndex, from, from, parentIndex, to)) {
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

template<typename ContactItem>
void AbstractContactModel::showContactMergeDialog(ContactItem *parent, ContactItem *child)
{
	if (!ServiceManager::getByName("MetaContactManager"))
		return;
	if (child->getContact() == parent->getContact())
		return;
	MetaContactManager * const manager = MetaContactManager::instance();
	if (!manager)
		return;
	MetaContact *childMeta = qobject_cast<MetaContact*>(child->getContact());
	MetaContact *meta = qobject_cast<MetaContact*>(parent->getContact());

	QString text;
	if (!childMeta && !meta) {
		text = tr("Would you like to merge contacts \"%1\" <%2> and \"%3\" <%4>?");
		text = text.arg(child->getContact()->name(),
						child->getContact()->id(),
						parent->getContact()->name(),
						parent->getContact()->id());
	} else if (childMeta && meta) {
		text = tr("Would you like to merge metacontacts \"%1\" and \"%2\"?");
		text = text.arg(childMeta->title(), meta->title());
	} else {
		text = tr("Would you like to add \"%1\" <%2> to metacontact \"%3\"?");
		Contact *c = (meta ? child : parent)->getContact();
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
			meta->addContact(parent->getContact());
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
			meta->addContact(child->getContact());
	}
}

}
}

#endif // ABSTRACTCONTACTMODEL_P_H

