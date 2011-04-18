/****************************************************************************
 *  abstractcontactmodel.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef ABSTRACTCONTACTMODEL_H
#define ABSTRACTCONTACTMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include "simplecontactlist_global.h"
#include "simplecontactlistitem.h"

namespace qutim_sdk_0_3
{
class Contact;
class Account;
}

namespace Core {
namespace SimpleContactList {


class SIMPLECONTACTLIST_EXPORT AbstractContactModel : public QAbstractItemModel
{
    Q_OBJECT
	Q_CLASSINFO("Service", "ContactModel")
public:
    explicit AbstractContactModel(QObject *parent = 0);
	virtual ~AbstractContactModel();
	virtual QStringList selectedTags() const;
	Q_INVOKABLE virtual QStringList tags() const;
public slots:
	virtual bool showOffline() const;
	virtual void hideShowOffline();
	virtual void filterList(const QString &name);
	virtual void filterList(const QStringList &tags);
signals:
	void tagVisibilityChanged(const QModelIndex &index, const QString &name, bool shown);
protected:
	template<typename TagContainer, typename TagItem, typename ContactItem>
	bool hideContact(ContactItem *item, bool hide, bool replacing = true);
	template<typename TagContainer, typename TagItem>
	void hideTag(TagItem *item);
	template<typename TagContainer, typename TagItem>
	void showTag(TagItem *item);
	template<typename TagItem, typename TagContainer>
	TagItem *ensureTag(TagContainer *p, const QString &name);
	template<typename TagContainer, typename ContactItem>
	void updateContact(ContactItem *item, bool placeChanged);
};

template<typename ContactItem>
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

template<typename TagContainer, typename TagItem, typename ContactItem>
bool AbstractContactModel::hideContact(ContactItem *item, bool hide, bool replacing)
{
	TagItem *tag = item->parent;
	TagContainer *p = tag->getTagContainer(this);
	Q_ASSERT(tag);
	Q_ASSERT(!replacing || tag->contacts.contains(item));
	if (!hide)
		showTag<TagContainer, TagItem>(tag);
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
			hideTag<TagContainer, TagItem>(tag);
	} else {
		Q_ASSERT(row >= 0);
		//Q_ASSERT(isVisible(item)); // TODO: uncomment me
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

template<typename TagContainer, typename ContactItem>
void AbstractContactModel::updateContact(ContactItem *item, bool placeChanged)
{
	QList<ContactItem *> &contacts = item->parent->visible;
	TagContainer *p = item->parent->getTagContainer(this);
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
			endMoveRows();
		}
	}
}


} // namespace SimpleContactList
} // namespace Core

#endif // ABSTRACTCONTACTMODEL_H
