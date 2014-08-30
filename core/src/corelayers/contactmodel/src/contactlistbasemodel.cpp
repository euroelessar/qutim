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
#include <qutim/metacontact.h>
#include <qutim/protocol.h>
#include <qutim/icon.h>
#include <qutim/event.h>
#include <qutim/accountmanager.h>

#include <QCoreApplication>
#include <QStringBuilder>
#include <QQueue>
#include <QtAlgorithms>
#include <QtDebug>

using namespace qutim_sdk_0_3;

ContactListBaseModel::ContactListBaseModel(QObject *parent) :
	QAbstractItemModel(parent), NotificationBackend("ContactList")
{
	setDescription(QT_TR_NOOP("Blink icon in the contact list"));
	allowRejectedNotifications("confMessageWithoutUserNick");

	m_showNotificationIcon = false;

	m_mailIcon = Icon(QLatin1String("mail-message-new-qutim"));
	m_typingIcon = Icon(QLatin1String("im-status-message-edit"));
	m_chatUserJoinedIcon = Icon(QLatin1String("list-add-user-conference"));
	m_chatUserLeftIcon = Icon(QLatin1String("list-remove-user-conference"));
	m_qutimIcon = Icon(QLatin1String("qutim"));
	m_transferCompletedIcon = Icon(QLatin1String("document-save-filetransfer-comleted"));
	m_birthdayIcon = Icon(QLatin1String("view-calendar-birthday"));
	m_defaultNotificationIcon = Icon(QLatin1String("dialog-information"));

	m_realAccountRequestId = Event::registerType("real-account-request");
	m_realUnitRequestId = Event::registerType("real-chatunit-request");
}

void ContactListBaseModel::initialize()
{
	AccountManager *manager = AccountManager::instance();
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
			return QString(account->name()
					% QLatin1Literal(" (")
					% account->id()
					% QLatin1Literal(")"));
		case ItemTypeRole:
			return AccountType;
		case Qt::DecorationRole:
			return account->status().icon();
		case AccountRole:
			return qVariantFromValue(account);
		case ContactsCountRole:
			return node->totalContacts.size();
		case OnlineContactsCountRole:
			return node->onlineContacts.size();
		case TagNameRole:
			return account->id();
		default:
			return QVariant();
		}
	} else if (TagNode *node = extractNode<TagNode>(index)) {
		switch (role) {
		case Qt::DisplayRole:
			return node->name;
		case ItemTypeRole:
			return TagType;
		case Qt::DecorationRole:
			return Icon("feed-subscribe");
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
		case Qt::DisplayRole: {
			const QString name = contact->name();
			return name.isEmpty() ? contact->id() : name;
		}
		case Qt::DecorationRole:
			if (m_showNotificationIcon) {
				if (Notification *notification = m_notificationHash.value(contact).value(0))
					return findNotificationIcon(notification);
			}
			return contact->status().icon();
		case ItemTypeRole:
			return ContactType;
		case StatusTextRole:
			return contact->status().text();
		case StatusRole:
			return qVariantFromValue(contact->status());
		case AvatarRole:
			return contact->avatar();
		case AlphabetRole:
			return contact->title().at(0).toUpper();
		case ContactRole:
			return qVariantFromValue<QObject*>(contact);
		case IdRole:
			return contact->id();
		case NotificationRole:
			if (Notification *notification = m_notificationHash.value(contact).value(0))
				return notification->request().type();
			return -1;
		case StatusIconNameRole:
			return contact->status().icon().name();
		case BuddyRole:
			return qVariantFromValue<Buddy*>(contact);
		default:
			return QVariant();
		}
	}
	return QVariant();
}

void ContactListBaseModel::handleNotification(Notification *notification)
{
	Contact *contact = findRealContact(notification);
	if (!contact || !m_contactHash.contains(contact))
		return;

	if (!m_notificationTimer.isActive()) {
		m_showNotificationIcon = true;
		m_notificationTimer.start(500, this);
	}

	NotificationList &queue = m_notificationHash[contact];
	NotificationList::Iterator it = qUpperBound(queue.begin(), queue.end(),
												notification, Comparator());
	queue.insert(it, notification);
	ref(notification);

	connect(notification, SIGNAL(finished(qutim_sdk_0_3::Notification::State)),
			this, SLOT(onNotificationFinished()));

	if (queue.first() == notification)
		onContactChanged(contact, true);
}

void ContactListBaseModel::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_notificationTimer.timerId()) {
		m_showNotificationIcon = !m_showNotificationIcon;

		for (NotificationHash::Iterator it = m_notificationHash.begin();
			 it != m_notificationHash.end();
			 ++it) {
			onContactChanged(it.key());
		}
		return;
	}
	return QAbstractItemModel::timerEvent(event);
}

void ContactListBaseModel::onNotificationFinished()
{
	Notification *notification = qobject_cast<Notification*>(sender());
	Q_ASSERT(notification);

	Contact *contact = findRealContact(notification);
	Q_ASSERT(contact);
	if (!m_contactHash.contains(contact))
		return;

	NotificationList &queue = m_notificationHash[contact];
	bool changed = (queue.first() == notification);
	queue.removeOne(notification);

	if (queue.isEmpty()) {
		m_notificationHash.remove(contact);
		if (m_notificationHash.isEmpty()) {
			m_showNotificationIcon = false;
			m_notificationTimer.stop();
		}
	}

	if (changed)
		onContactChanged(contact, true);
}

QIcon ContactListBaseModel::findNotificationIcon(Notification *notification) const
{
	const NotificationRequest request = notification->request();
	switch (request.type()) {
	case Notification::IncomingMessage:
	case Notification::OutgoingMessage:
	case Notification::ChatIncomingMessage:
	case Notification::ChatOutgoingMessage:
		return m_mailIcon;
	case Notification::UserTyping:
		return m_typingIcon;
	case Notification::UserOnline:
	case Notification::UserOffline:
	case Notification::UserChangedStatus:
		return request.property("previousStatus", Status(Status::Offline)).icon();
	case Notification::ChatUserJoined:
		return m_chatUserJoinedIcon;
	case Notification::ChatUserLeft:
		return m_chatUserLeftIcon;
	case Notification::AppStartup:
		return m_qutimIcon;
	case Notification::FileTransferCompleted:
		return m_transferCompletedIcon;
	case Notification::UserHasBirthday:
		return m_birthdayIcon;
	case Notification::BlockedMessage:
	case Notification::System:
	case Notification::Attention:
		return m_defaultNotificationIcon;
	}
	return QIcon();
}

int ContactListBaseModel::findNotificationPriority(Notification *notification)
{;
	const NotificationRequest request = notification->request();
	switch (request.type()) {
	case Notification::IncomingMessage:
	case Notification::OutgoingMessage:
	case Notification::ChatIncomingMessage:
	case Notification::ChatOutgoingMessage:
		return 10;
	case Notification::UserTyping:
		return 0;
	default:
		return 5;
	}
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
		foreach (Contact *contact, account->findChildren<Contact*>()) {
			if (!contact->metaContact())
				onContactAdded(contact);
			if (MetaContact *metaContact = qobject_cast<MetaContact*>(contact)) {
				foreach (ChatUnit *unit, metaContact->lowerUnits()) {
					if (Contact *subContact = qobject_cast<Contact*>(unit))
						onContactRemoved(subContact);
				}
			}
		}
	}

	connect(account, SIGNAL(destroyed(QObject*)),
			this, SLOT(onAccountDestroyed(QObject*)));
	connect(account, SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			this, SLOT(onContactAdded(qutim_sdk_0_3::Contact*)));
}

void ContactListBaseModel::onAccountDestroyed(QObject *obj)
{
	removeAccountNode(static_cast<Account*>(obj), &m_root);
}

void ContactListBaseModel::onAccountRemoved(Account *account)
{
	disconnect(account);

	// TODO: Check if it works
	foreach (MetaContact *metaContact, account->findChildren<MetaContact*>()) {
		foreach (ChatUnit *unit, metaContact->lowerUnits()) {
			if (Contact *contact = qobject_cast<Contact*>(unit))
				onContactAdded(contact);
		}
	}

	removeAccount(account);

	removeAccountNode(account, &m_root);
}

void ContactListBaseModel::onContactDestroyed(QObject *obj)
{
	Contact *contact = static_cast<Contact*>(obj);

	if (m_notificationHash.remove(contact) > 0 && m_notificationHash.isEmpty())
		m_notificationTimer.stop();

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

void ContactListBaseModel::onContactAdded(Contact *contact)
{
	addTags(contact->tags());

	addContact(contact);

	connectContact(contact);
}

void ContactListBaseModel::onContactRemoved(Contact *contact)
{
	if (m_notificationHash.remove(contact) > 0 && m_notificationHash.isEmpty())
		m_notificationTimer.stop();

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

void ContactListBaseModel::onStatusChanged(const Status &current, const Status &previous)
{
	Contact *contact = static_cast<Contact*>(sender());
	const bool wasOnline = (previous != Status::Offline);
	const bool online = (current != Status::Offline);

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
	account = findRealAccount(account);

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

		const bool online = (contact->status() != Status::Offline);
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

		const bool online = (contact->status() != Status::Offline);
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

Account *ContactListBaseModel::findRealAccount(Account *account)
{
	Event event(m_realAccountRequestId);
	QCoreApplication::sendEvent(account, &event);
	Account *realAccount = event.at<Account*>(0);
	return realAccount ? realAccount : account;
}

Contact *ContactListBaseModel::findRealContact(Notification *notification)
{
	const char *contactProperty = "contactList_realContact";
	if (Contact *contact = notification->property(contactProperty).value<Contact*>())
		return contact;

	ChatUnit *unit = qobject_cast<ChatUnit*>(notification->request().object());
	if (!unit)
		return 0;

	Event event(m_realUnitRequestId);
	QCoreApplication::sendEvent(unit, &event);

	Contact *contact = event.at<Contact*>(0);
	while (unit && !contact) {
		if ((contact = qobject_cast<Contact*>(unit)))
			break;
		unit = unit->upperUnit();
	}

	if (Contact *meta = qobject_cast<MetaContact*>(contact ? contact->metaContact() : 0))
		contact = meta;

	notification->setProperty(contactProperty, qVariantFromValue(contact));
	return contact;
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

static inline bool fix_hash(QHash<qutim_sdk_0_3::Contact*, int> &contacts, qutim_sdk_0_3::Contact *contact, int delta)
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
	connect(contact, SIGNAL(destroyed(QObject*)),
			this, SLOT(onContactDestroyed(QObject*)));
	connect(contact, SIGNAL(tagsChanged(QStringList,QStringList)),
			this, SLOT(onContactTagsChanged(QStringList,QStringList)));
	connect(contact, SIGNAL(inListChanged(bool)),
			this, SLOT(onContactChanged()));
	connect(contact, SIGNAL(titleChanged(QString,QString)),
			this, SLOT(onContactChanged()));
	connect(contact, SIGNAL(avatarChanged(QString)),
			this, SLOT(onContactChanged()));
	connect(contact, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
	m_comparator->startListen(contact);
}

void ContactListBaseModel::disconnectContact(Contact *contact)
{
	disconnect(contact);
	m_comparator->stopListen(contact);
}
