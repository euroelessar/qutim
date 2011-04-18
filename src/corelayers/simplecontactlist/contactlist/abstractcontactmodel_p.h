#ifndef ABSTRACTCONTACTMODEL_P_H
#define ABSTRACTCONTACTMODEL_P_H

#include "abstractcontactmodel.h"
#include "simplecontactlistitem.h"
#include <QBasicTimer>
#include <qutim/icon.h>

namespace qutim_sdk_0_3
{
	class ChatSession;
}

#define QUTIM_MIME_CONTACT_INTERNAL QLatin1String("application/qutim-contact-internal")
#define QUTIM_MIME_TAG_INTERNAL QLatin1String("application/qutim-tag-internal")

namespace Core {
namespace SimpleContactList {

struct ChangeEvent
{
	enum Type { ChangeTags, MergeContacts, MoveTag } type;
	void *child;
	ItemHelper *parent;

};

class AbstractContactModelPrivate
{
public:
	QStringList selectedTags;
	QString lastFilter;
	QList<ChangeEvent*> events;
	QMap<ChatSession*, QSet<Contact*> > unreadBySession;
	QSet<Contact*> unreadContacts;
	QBasicTimer timer;
	QBasicTimer unreadTimer;
	QIcon unreadIcon;
	quint16 realUnitRequestEvent;
	bool showMessageIcon;
	bool showOffline;
};

template<typename ContactItem>
bool contactLessThan(ContactItem *a, ContactItem *b) {
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

template<typename TagContainer, typename TagItem, typename ContactItem>
bool AbstractContactModel::hideContact(ContactItem *item, bool hide, bool replacing)
{
	TagItem *tag = item->parent;
	Q_ASSERT(tag);
	Q_ASSERT(!replacing || tag->contacts.contains(item));
	if (!hide)
		showTag<TagContainer, TagItem>(tag);
	int row = item->parentIndex(this);
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
	beginInsertRows(QModelIndex(), index, index);
	p->visibleTags.insert(index, item);
	endInsertRows();
	emit tagVisibilityChanged(createIndex(index, 0, item), item->name, true);
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
	QList<ContactItem *> &contacts = item->parent->visible;
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

	QModelIndex parentIndex = createIndex(item->parentIndex(this), 0, item->parent);

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
			endMoveRows();
		}
	}
}

template<typename ContactItem>
QVariant AbstractContactModel::contactData(const QModelIndex &index, int role) const
{
	Q_D(const AbstractContactModel);
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
		return d->showOffline || item->data->status.type() != Status::Offline;
	}
}

} }

#endif // ABSTRACTCONTACTMODEL_P_H
