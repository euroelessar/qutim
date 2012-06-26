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

#include "contactlistmodel.h"
#include "contactlist.h"

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;
enum {
	IdRole = Qt::UserRole,
	ContactRole,
	AlphabetRole,
	StatusTextRole,
	AvatarRole
};

ContactListModel::ContactListModel()
{
	QHash<int, QByteArray> roleNames;
	roleNames.insert(IdRole, "id");
	roleNames.insert(Qt::DisplayRole, "title");
	roleNames.insert(Qt::DecorationRole, "iconSource");
	roleNames.insert(ContactRole, "contact");
	roleNames.insert(AlphabetRole, "alphabet");
	roleNames.insert(StatusTextRole, "subtitle");
	roleNames.insert(AvatarRole, "avatar");
	setRoleNames(roleNames);
	foreach (Protocol *protocol, Protocol::all()) {
		connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
		        SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach (Account *account, protocol->accounts())
			onAccountCreated(account);
	}
	m_statusPrefix = QLatin1String("icon-m-common");
}

ContactListModel::~ContactListModel()
{
}

QString ContactListModel::statusPrefix()
{
	return m_statusPrefix;
}

void ContactListModel::setStatusPrefix(const QString &prefix)
{
	if (m_statusPrefix == prefix)
		return;
	m_statusPrefix = prefix;
	emit statusPrefixChanged(m_statusPrefix);
}

QString ContactListModel::filter() const
{
	return m_filter;
}

void ContactListModel::setFilter(const QString &filter)
{
	if (m_filter == filter)
		return;
	m_filter = filter;
	emit filterChanged(m_filter);
	qDebug() << m_filter << m_contacts.size();
	checkVisibility();
	qDebug() << m_filter << m_contacts.size();
}

bool ContactListModel::showOffline() const
{
	return m_showOffline;
}

void ContactListModel::setShowOffline(bool showOffline)
{
	if (m_showOffline == showOffline)
		return;
	m_showOffline = showOffline;
	emit showOfflineChanged(m_showOffline);
	checkVisibility();
}

// I hope it will be invoked only by SectionScroller
QVariant ContactListModel::get(int index)
{
	QVariantMap map;
	Contact *contact = m_contacts.value(index).contact;
	const QString title = contact ? contact->title() : QString();
	const QString alphabet(1, title.isEmpty() ? QChar() : title.at(0).toUpper());
	map.insert(QLatin1String("alphabet"), alphabet);
	return map;
}

int ContactListModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_contacts.size();
}

QVariant ContactListModel::data(const QModelIndex &index, int role) const
{
	if (index.row() >= m_contacts.size())
		return QVariant();
	Contact *contact = m_contacts[index.row()].contact;
	switch (role) {
	case IdRole:
		return contact->id();
	case Qt::DisplayRole:
		return contact->title();
	case Qt::DecorationRole:
		return ContactList::statusUrl(contact->status().type(), m_statusPrefix);
	case ContactRole:
		return qVariantFromValue<QObject*>(contact);
	case AlphabetRole:
		return contact->title().at(0).toUpper();
	case StatusTextRole:
		return contact->status().text();
	case AvatarRole:
		return contact->avatar();
	default:
		return QVariant();
	}
}

void ContactListModel::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	connect(account, SIGNAL(destroyed(QObject*)),
	        SLOT(onAccountDeath(QObject*)));

	connect(account, SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
				this, SLOT(onContactCreated(qutim_sdk_0_3::Contact*)));
	foreach (Contact *contact, account->findChildren<Contact*>())
		onContactCreated(contact);

}

void ContactListModel::onAccountDeath(QObject *object)
{
	Q_UNUSED(object);
}

bool ContactListModel::Item::operator <(const ContactListModel::Item &o) const
{
	qptrdiff cmp = title.compare(o.title, Qt::CaseInsensitive);
	if (cmp == 0)
		cmp = contact - o.contact;
	return cmp < 0;
}

void ContactListModel::onContactCreated(qutim_sdk_0_3::Contact *contact)
{
	connect(contact, SIGNAL(destroyed(QObject*)),
	        SLOT(onContactDeath(QObject*)));
	connect(contact, SIGNAL(titleChanged(QString,QString)),
	        SLOT(onContactTitleChanged(QString,QString)));
	connect(contact, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
	        SLOT(onContactStatusChanged(qutim_sdk_0_3::Status)));
	m_titles.insert(contact, contact->title());
	checkVisibility(contact);
}

void ContactListModel::onContactTitleChanged(const QString &title, const QString &oldTitle)
{
	Contact *contact = static_cast<Contact*>(sender());
	Item oldItem = { oldTitle, contact };
	Item item = { title, contact };
	m_titles.insert(item.contact, item.title);
	QList<Item>::Iterator oldIt, it;
	oldIt = qBinaryFind(m_contacts.begin(), m_contacts.end(), oldItem);
	if (oldIt == m_contacts.end()) {
		checkVisibility(contact);
		return;
	}
	int oldIndex = oldIt - m_contacts.begin();
	if (isVisible(contact)) {
		it = qLowerBound(m_contacts.begin(), m_contacts.end(), item);
	} else {
		beginRemoveRows(QModelIndex(), oldIndex, oldIndex);
		m_contacts.removeAt(oldIndex);
		endRemoveRows();
		emit countChanged(m_contacts.size());
		return;
	}
	if (oldIt == it)
		return;
	int index = it - m_contacts.begin();
	beginMoveRows(QModelIndex(), oldIndex, oldIndex, QModelIndex(), index);
	m_contacts.move(oldIndex, index);
	endMoveRows();
	emit countChanged(m_contacts.size());
}

void ContactListModel::onContactStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Q_UNUSED(status);
	checkVisibility(static_cast<Contact*>(sender()));
}

void ContactListModel::onContactDeath(QObject *object)
{
	Item item = { m_titles.take(object), static_cast<Contact*>(object) };
	QList<Item>::Iterator it;
	it = qBinaryFind(m_contacts.begin(), m_contacts.end(), item);
	int index = it - m_contacts.begin();
	beginRemoveRows(QModelIndex(), index, index);
	m_contacts.removeAt(index);
	endRemoveRows();
	emit countChanged(m_contacts.size());
}

void ContactListModel::checkVisibility()
{
	QHash<QObject*, QString>::ConstIterator it = m_titles.constBegin();
	for (; it != m_titles.constEnd(); ++it)
		checkVisibility(static_cast<Contact*>(it.key()), true);
	emit countChanged(m_contacts.size());
}

void ContactListModel::checkVisibility(qutim_sdk_0_3::Contact *contact, bool forced)
{
	int index = indexOfContact(contact);
	bool visible = m_contacts.value(index).contact == contact;
	bool newVisible = isVisible(contact);
	if (newVisible == visible) {
		if (!forced && visible) {
			QModelIndex modelIndex = createIndex(index, 0, &m_contacts[index]);
			emit dataChanged(modelIndex, modelIndex);
		}
		return;
	}
	if (newVisible) {
		Item item = { contact->title(), contact };
		beginInsertRows(QModelIndex(), index, index);
		m_contacts.insert(index, item);
		endInsertRows();
	} else {
		beginRemoveRows(QModelIndex(), index, index);
		m_contacts.removeAt(index);
		endRemoveRows();
	}
	if (!forced)
		emit countChanged(m_contacts.size());
}

bool ContactListModel::isVisible(qutim_sdk_0_3::Contact *contact)
{
	if (!m_filter.isEmpty()) {
		return contact->id().contains(m_filter, Qt::CaseInsensitive)
		        || contact->name().contains(m_filter, Qt::CaseInsensitive);
	} else if (!m_showOffline) {
		return contact->status() != Status::Offline;
	} else {
		return true;
	}
}

int ContactListModel::indexOfContact(qutim_sdk_0_3::Contact *contact) const
{
	const Item item = { contact->title(), contact };
	return qLowerBound(m_contacts.begin(), m_contacts.end(), item)
	        - m_contacts.begin();
}
}

