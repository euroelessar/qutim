/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "contactlistbasemodel.h"

#include <QCoreApplication>
#include <QStringBuilder>
#include <QQueue>
#include <QtAlgorithms>
#include <QtDebug>

ContactListBaseModel::ContactListBaseModel(QObject *parent) :
	QAbstractItemModel(parent)
{
}

QModelIndex ContactListBaseModel::index(int row, int column, const QModelIndex &parent) const
{
	Q_UNUSED(column);

	if (row < 0)
		return QModelIndex();

	int index = row;

	if (ContactListNode *node = extractNode<ContactListNode>(parent)) {
		if (index >= node->contacts.size())
			index -= node->contacts.size();
		else
			return createIndex(node->contacts[index], row);
	}

	if (TagListNode *node = extractNode<TagListNode>(parent)) {
		if (index >= node->tags.size())
			index -= node->tags.size();
		else
			return createIndex(node->tags[index], row);
	}

	if (AccountListNode *node = extractNode<AccountListNode>(parent)) {
		if (index >= node->accounts.size())
			index -= node->accounts.size();
		else
			return createIndex(node->accounts[index], row);
	}

	return QModelIndex();
}

QModelIndex ContactListBaseModel::parent(const QModelIndex &child) const
{
	if (BaseNode *node = extractNode(child))
		return createIndex(node->parent());
	return QModelIndex();
}

int ContactListBaseModel::rowCount(const QModelIndex &parent) const
{
	int count = 0;

	if (ContactListNode *node = extractNode<ContactListNode>(parent))
		count += node->contacts.size();
	if (TagListNode *node = extractNode<TagListNode>(parent))
		count += node->tags.size();
	if (AccountListNode *node = extractNode<AccountListNode>(parent))
		count += node->accounts.size();

	return count;
}

int ContactListBaseModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 1;
}

bool ContactListBaseModel::hasChildren(const QModelIndex &parent) const
{
	return rowCount(parent) > 0;
}

QVariant ContactListBaseModel::data(const QModelIndex &index, int role) const
{
	if (AccountNode *node = extractNode<AccountNode>(index)) {
		if (!node->account)
			return QVariant();
		Account *account = node->account.data();
		switch (role) {
		case Qt::DisplayRole:
			return account->nickname();
		case ItemTypeRole:
			return AccountType;
		case Qt::DecorationRole:
            return QVariant();
//			return account->presence().icon();
		case AccountRole:
			return qVariantFromValue(account);
		case ContactsCountRole:
			return node->totalContacts.size();
		case OnlineContactsCountRole:
			return node->onlineContacts.size();
		case TagNameRole:
			return account->uniqueIdentifier();
		default:
			return QVariant();
		}
	} else if (TagNode *node = extractNode<TagNode>(index)) {
		switch (role) {
		case Qt::DisplayRole:
			return node->name;
		case ItemTypeRole:
			return TagType;
//		case Qt::DecorationRole:
//			return Icon("feed-subscribe");
		case ContactsCountRole:
			return node->totalContacts.size();
		case OnlineContactsCountRole:
			return node->onlineContacts.size();
		case TagNameRole:
			return node->name;
		default:
			return QVariant();
		}
	} else if (ContactNode *node = extractNode<ContactNode>(index)) {
		if (!node->contact)
			return QVariant();
		Contact *contact = node->contact.data();
		switch (role) {
		case Qt::EditRole:
		case Qt::DisplayRole:
            return contact->alias();
		case Qt::DecorationRole:
            return QVariant();
//			if (m_showNotificationIcon) {
//				if (Notification *notification = m_notificationHash.value(contact).value(0))
//					return findNotificationIcon(notification);
//			}
//			return contact->presence().icon();
		case ItemTypeRole:
			return ContactType;
		case StatusTextRole:
			return contact->presence().statusMessage();
		case StatusRole:
			return qVariantFromValue(contact->presence());
        case StatusTypeRole:
            return contact->presence().type();
		case AvatarRole:
			return contact->avatar();
		case AlphabetRole:
			return contact->alias().at(0).toUpper();
		case ContactRole:
			return qVariantFromValue(contact);
		case IdRole:
			return contact->id();
//		case NotificationRole:
//			if (Notification *notification = m_notificationHash.value(contact).value(0))
//				return notification->request().type();
//			return -1;
		case StatusIconNameRole:
			return contact->presence().status();
		default:
			return QVariant();
		}
	}
	return QVariant();
}

QStringList ContactListBaseModel::tags() const
{
	return m_tags;
}

void ContactListBaseModel::updateContactTags(Contact *contact, const QStringList &current, const QStringList &previous)
{
	Q_UNUSED(contact);
	Q_UNUSED(current);
	Q_UNUSED(previous);
}

void ContactListBaseModel::addAccount(Account *account)
{
	Q_UNUSED(account);
}

void ContactListBaseModel::removeAccount(Account *account)
{
	Q_UNUSED(account);
}

void ContactListBaseModel::onAccountCreated(Account *account, bool addContacts)
{
	addAccount(account);

	if (addContacts) {
        foreach (Contact *contact, account->contactManager()->allKnownContacts())
            onContactAdded(contact);
	}

	connect(account, &Account::destroyed,
			this, &ContactListBaseModel::onAccountDestroyed);
	connect(account->contactManager(), &ContactManager::allKnownContactsChanged,
			this, &ContactListBaseModel::onContactsChanged);
}

void ContactListBaseModel::onAccountDestroyed(QObject *obj)
{
	removeAccountNode(static_cast<Account*>(obj), &m_root);
}

void ContactListBaseModel::onAccountRemoved(Account *account)
{
	disconnect(account);

	removeAccount(account);

	removeAccountNode(account, &m_root);
}

void ContactListBaseModel::onContactDestroyed(QObject *obj)
{
	Contact *contact = static_cast<Contact*>(obj);

	ContactHash::Iterator it = m_contactHash.find(contact);

	if (it != m_contactHash.end()) {
		QList<ContactNode*> contacts = *it;
		m_contactHash.erase(it);

		foreach (ContactNode *node, contacts) {
			ContactListNode *parentNode = node->parent();
			QModelIndex parentIndex = createIndex(node->parent());
			QList<ContactNode>::iterator jt = qBinaryFind(parentNode->contacts.begin(),
														  parentNode->contacts.end(),
														  contact,
														  Comparator());
			Q_ASSERT(jt != parentNode->contacts.end());
			int index = jt - parentNode->contacts.begin();
			beginRemoveRows(parentIndex, index, index);
			parentNode->contacts.erase(jt);
			endRemoveRows();
		}
    }
}

void ContactListBaseModel::onContactsChanged(const QList<Contact *> &contactsAdded, const QList<Contact *> &contactsRemoved)
{
    for (auto contact : contactsAdded)
        onContactAdded(contact);
    for (auto contact : contactsRemoved)
        onContactRemoved(contact);
}

void ContactListBaseModel::onContactAdded(Contact *contact)
{
	addTags(contact->groups());

	addContact(contact);

	connectContact(contact);
}

void ContactListBaseModel::onContactRemoved(Contact *contact)
{
	removeContact(contact);

	disconnectContact(contact);
}

void ContactListBaseModel::onContactChanged(Contact *contact, bool parentsChanged)
{
	ContactHash::Iterator it = m_contactHash.find(contact);

	QSet<BaseNode*> used;
	QQueue<BaseNode*> queue;

	if (it != m_contactHash.end()) {
		if (!parentsChanged) {
			foreach (ContactNode *node, *it) {
				QModelIndex index = createIndex(node);
				dataChanged(index, index);
			}
		} else {
			// Restricted nodes
			used << NULL << &m_root;

			foreach (ContactNode *node, *it) {
				used << node;
				queue.enqueue(node);
			}

			while (!queue.isEmpty()) {
				BaseNode *node = queue.dequeue();
				QModelIndex index = createIndex(node);
				dataChanged(index, index);

				if (!used.contains(node->parent())) {
					used << node->parent();
					queue.enqueue(node->parent());
				}
			}
		}
	}
}

void ContactListBaseModel::onContactChanged()
{
	if (Contact *contact = qobject_cast<Contact*>(sender()))
		onContactChanged(contact);
}

void ContactListBaseModel::onContactTagsChanged(const QStringList &current, const QStringList &previous)
{
	addTags(current);

	if (Contact *contact = qobject_cast<Contact*>(sender())) {
		updateContactTags(contact, current, previous);
		onContactChanged(contact);
	}
}

void ContactListBaseModel::onStatusChanged(const Presence &current, const Presence &previous)
{
	Contact *contact = static_cast<Contact*>(sender());
	const bool wasOnline = previous.isOnline();
	const bool online = current.isOnline();

	if (online == wasOnline) {
		onContactChanged(contact);
	} else {
		ContactHash::Iterator it = m_contactHash.find(contact);
		if (it != m_contactHash.end()) {
			foreach (ContactNode *node, *it) {
				QModelIndex contactIndex = createIndex(node);
				dataChanged(contactIndex, contactIndex);

				updateItemCount(contact, node->parent(), online ? 1 : -1, 0);
			}
		}
	}
}

ContactListBaseModel::AccountNode *ContactListBaseModel::ensureAccount(Account *account, ContactListBaseModel::AccountListNode *parent)
{
	QModelIndex index = createIndex(parent);

	for (int i = 0; i < parent->accounts.size(); ++i) {
		if (parent->accounts[i].account == account)
			return &parent->accounts[i];
	}

	beginInsertRows(index, parent->accounts.size(), parent->accounts.size());
	parent->accounts.append(AccountNode(account, m_root));
	AccountNode *node = &parent->accounts.last();
	endInsertRows();

	return node;
}

void ContactListBaseModel::eraseAccount(Account *account, ContactListBaseModel::AccountListNode *parent)
{
	removeAccountNode(account, parent);
	foreach (Contact *contact, account->findChildren<Contact*>())
		disconnectContact(contact);
}

ContactListBaseModel::TagNode *ContactListBaseModel::ensureTag(const QString &name, ContactListBaseModel::TagListNode *parent)
{
	QModelIndex parentIndex = createIndex(parent);

	QList<TagNode>::iterator it = qLowerBound(parent->tags.begin(),
											  parent->tags.end(),
											  name,
											  Comparator());
	if (it != parent->tags.end() && it->name == name)
		return &*it;
	int index = it - parent->tags.begin();

	beginInsertRows(parentIndex, index, index);
	it = parent->tags.insert(it, TagNode(name, *parent));
	endInsertRows();

	return &*it;
}

ContactListBaseModel::ContactNode *ContactListBaseModel::ensureContact(Contact *contact, ContactListBaseModel::ContactListNode *parent)
{
	QModelIndex parentIndex = createIndex(parent);

	QList<ContactNode>::iterator it = qLowerBound(parent->contacts.begin(),
												  parent->contacts.end(),
												  contact,
												  Comparator());
	Q_ASSERT(it == parent->contacts.end() || it->contact != contact);
	if (it == parent->contacts.end() || it->contact != contact) {
		int index = it - parent->contacts.begin();

		beginInsertRows(parentIndex, index, index);
		it = parent->contacts.insert(it, ContactNode(contact, *parent));
		ContactNode &node = *it;
		m_contactHash[contact].append(&node);
		Q_ASSERT(m_contactHash[contact].count(&node) == 1);
		endInsertRows();

		const bool online = contact->presence().isOnline();
		updateItemCount(contact, parent, online ? 1 : 0, 1);
	}

	return &*it;
}

void ContactListBaseModel::eraseContact(Contact *contact, ContactListBaseModel::ContactListNode *parent)
{
	QModelIndex parentIndex = createIndex(parent);

	QList<ContactNode>::iterator it = qBinaryFind(parent->contacts.begin(),
												  parent->contacts.end(),
												  contact,
												  Comparator());

	Q_ASSERT(it != parent->contacts.end());
	if (it != parent->contacts.end()) {
		int index = it - parent->contacts.begin();
		beginRemoveRows(parentIndex, index, index);
		ContactNode &node = *it;
		ContactHash::Iterator jt = m_contactHash.find(contact);
		Q_ASSERT(jt != m_contactHash.end());
		jt->removeOne(&node);
		if (jt->isEmpty())
			m_contactHash.erase(jt);
		parent->contacts.erase(it);
		endRemoveRows();

		const bool online = contact->presence().isOnline();
		updateItemCount(contact, parent, online ? -1 : 0, -1);
	}
}

ContactListBaseModel::RootNode *ContactListBaseModel::rootNode() const
{
	return const_cast<RootNode*>(&m_root);
}

QStringList ContactListBaseModel::emptyTags() const
{
	if (m_emptyTags.isEmpty())
		m_emptyTags << tr("Without tags");
	return m_emptyTags;
}

QStringList ContactListBaseModel::fixTags(const QStringList &tags) const
{
	return tags.isEmpty() ? emptyTags() : tags;
}

bool ContactListBaseModel::findNode(ContactListBaseModel::BaseNode *node) const
{
	return findNode(node, const_cast<RootNode*>(&m_root));
}

bool ContactListBaseModel::findNode(ContactListBaseModel::BaseNode *node, ContactListBaseModel::BaseNode *current) const
{
	if (node == current)
		return true;

	if (ContactListNode *list = node_cast<ContactListNode*>(current)) {
		for (int i = 0; i < list->contacts.size(); ++i) {
			if (findNode(node, &list->contacts[i]))
				return true;
		}
	}
	if (TagListNode *list = node_cast<TagListNode*>(current)) {
		for (int i = 0; i < list->tags.size(); ++i) {
			if (findNode(node, &list->tags[i]))
				return true;
		}
	}
	if (AccountListNode *list = node_cast<AccountListNode*>(current)) {
		for (int i = 0; i < list->accounts.size(); ++i) {
			if (findNode(node, &list->accounts[i]))
				return true;
		}
	}
	return false;
}

void ContactListBaseModel::findContacts(QSet<Contact *> &contacts, ContactListBaseModel::BaseNode *current)
{
	if (ContactListNode *list = node_cast<ContactListNode*>(current)) {
		for (int i = 0; i < list->contacts.size(); ++i)
			contacts.insert(list->contacts[i].contact.data());
	}
	if (TagListNode *list = node_cast<TagListNode*>(current)) {
		for (int i = 0; i < list->tags.size(); ++i)
			findContacts(contacts, &list->tags[i]);
	}
	if (AccountListNode *list = node_cast<AccountListNode*>(current)) {
		for (int i = 0; i < list->accounts.size(); ++i)
			findContacts(contacts, &list->accounts[i]);
	}
}

void ContactListBaseModel::addTags(const QStringList &tags)
{
	bool modified = false;
	foreach (const QString &tag, tags) {
		QStringList::Iterator it = qLowerBound(m_tags.begin(), m_tags.end(), tag);
		if (it != m_tags.end() && *it == tag)
			continue;
		m_tags.insert(it, tag);
		modified = true;
	}
	if (modified)
		emit tagsChanged(m_tags);
}

static inline bool fix_hash(QHash<Contact*, int> &contacts, Contact *contact, int delta)
{
	if (delta == 0)
		return false;
	const int count = contacts.size();
	int &itemOnlineCount = contacts[contact];
	itemOnlineCount += delta;
	Q_ASSERT(itemOnlineCount >= 0);
	if (itemOnlineCount <= 0)
		contacts.remove(contact);
	return (count != contacts.size());
}

void ContactListBaseModel::updateItemCount(Contact *contact, ContactListBaseModel::ContactListNode *parent, int online, int total)
{
	while (parent && parent != &m_root) {
		bool modified = false;
		modified |= fix_hash(parent->onlineContacts, contact, online);
		modified |= fix_hash(parent->totalContacts, contact, total);

		if (modified) {
			QModelIndex index = createIndex(parent);
			dataChanged(index, index);
		}
		parent = node_cast<ContactListNode *>(parent->parent());
	}
}

void ContactListBaseModel::removeAccountNode(Account *account, ContactListBaseModel::BaseNode *parent)
{
	if (AccountListNode *node = node_cast<AccountListNode*>(parent)) {
		for (int index = 0; index < node->accounts.size(); ++index) {
			AccountNode *accountNode = &node->accounts[index];
			if (accountNode->account == account) {
				beginRemoveRows(createIndex(parent), index, index);
				clearContacts(accountNode);
				node->accounts.removeAt(index);
				endRemoveRows();
				return;
			}
		}
	}
}

void ContactListBaseModel::clearContacts(ContactListBaseModel::BaseNode *current)
{
	if (ContactListNode *list = node_cast<ContactListNode*>(current)) {
		for (int i = 0; i < list->contacts.size(); ++i)
			m_contactHash.remove(list->contacts[i].contact.data());
	}
	if (TagListNode *list = node_cast<TagListNode*>(current)) {
		for (int i = 0; i < list->tags.size(); ++i)
			clearContacts(&list->tags[i]);
	}
	if (AccountListNode *list = node_cast<AccountListNode*>(current)) {
		for (int i = 0; i < list->accounts.size(); ++i)
			clearContacts(&list->accounts[i]);
	}
}

QModelIndex ContactListBaseModel::createIndex(ContactListBaseModel::BaseNode *node) const
{
	Q_ASSERT(findNode(node));
	if (AccountNode *accountNode = node_cast<AccountNode*>(node)) {
		QList<AccountNode> &accounts = accountNode->parent()->accounts;
		for (int index = 0; index < accounts.size(); ++index) {
			if (accounts[index] == *accountNode) {
				return createIndex(accounts[index],
								   index
								   + accountNode->parent()->contacts.size()
								   + accountNode->parent()->tags.size());
			}
		}
	} else if (TagNode *tagNode = node_cast<TagNode*>(node)) {
		QList<TagNode> &tags = tagNode->parent()->tags;
		QList<TagNode>::iterator it = qBinaryFind(tags.begin(), tags.end(), *tagNode);
		if (it != tags.end()) {
			int index = it - tags.begin();
			return createIndex(*it, index + tagNode->parent()->contacts.size());
		}
	} else if (ContactNode *contactNode = node_cast<ContactNode*>(node)) {
		QList<ContactNode> &contacts = contactNode->parent()->contacts;
		QList<ContactNode>::iterator it = qBinaryFind(contacts.begin(), contacts.end(), *contactNode);
		if (it != contacts.end()) {
			int index = it - contacts.begin();
			return createIndex(*it, index);
		}
	}
	return QModelIndex();
}

ContactListBaseModel::BaseNode *ContactListBaseModel::extractNode(const QModelIndex &index) const
{
	if (!index.isValid())
		return const_cast<RootNode*>(&m_root);
	if (index.isValid() && index.model() == this)
		return reinterpret_cast<BaseNode*>(index.internalPointer());
	return NULL;
}

void ContactListBaseModel::connectContact(Contact *contact)
{
    auto onContactChanged = static_cast<void (ContactListBaseModel::*) ()>(
                &ContactListBaseModel::onContactChanged);

	connect(contact, &Contact::destroyed,
			this, &ContactListBaseModel::onContactDestroyed);
	connect(contact, &Contact::groupsChanged,
			this, &ContactListBaseModel::onContactTagsChanged);
	connect(contact, &Contact::inListChanged,
			this, onContactChanged);
	connect(contact, &Contact::aliasChanged,
			this, onContactChanged);
	connect(contact, &Contact::avatarChanged,
			this, onContactChanged);
	connect(contact, &Contact::presenceChanged,
			this, &ContactListBaseModel::onStatusChanged);
//	m_comparator->startListen(contact);
}

void ContactListBaseModel::disconnectContact(Contact *contact)
{
	disconnect(contact);
//	m_comparator->stopListen(contact);
}
