/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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
	StatusTextRole
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
	setRoleNames(roleNames);
	foreach (Protocol *protocol, Protocol::all()) {
		connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
		        SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach (Account *account, protocol->accounts())
			onAccountCreated(account);
	}
}

ContactListModel::~ContactListModel()
{
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
		return ContactList::statusUrl(contact->status().type());
	case ContactRole:
		return qVariantFromValue<QObject*>(contact);
	case AlphabetRole:
		return contact->title().at(0);
	case StatusTextRole:
		return contact->status().text();
	default:
		return QVariant();
	}
}

void ContactListModel::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	connect(account, SIGNAL(destroyed(QObject*)),
	        SLOT(onAccountDeath(QObject*)));
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
	Item item = { contact->title(), contact };
	m_titles.insert(item.contact, item.title);
	QList<Item>::Iterator it;
	it = qLowerBound(m_contacts.begin(), m_contacts.end(), item);
	int index = it - m_contacts.begin();
	beginInsertRows(QModelIndex(), index, index);
	m_contacts.insert(it, item);
	endInsertRows();
}

void ContactListModel::onContactTitleChanged(const QString &title, const QString &oldTitle)
{
	Contact *contact = static_cast<Contact*>(sender());
	Item oldItem = { oldTitle, contact };
	Item item = { title, contact };
	m_titles.insert(item.contact, item.title);
	QList<Item>::Iterator oldIt, it;
	oldIt = qLowerBound(m_contacts.begin(), m_contacts.end(), oldItem);
	it = qLowerBound(m_contacts.begin(), m_contacts.end(), item);
	if (oldIt == it)
		return;
	int oldIndex = oldIt - m_contacts.begin();
	int index = it - m_contacts.begin();
	beginMoveRows(QModelIndex(), oldIndex, oldIndex, QModelIndex(), index);
	m_contacts.move(oldIndex, index);
	endMoveRows();
}

void ContactListModel::onContactStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Q_UNUSED(status);
	QObject *object = sender();
	Item item = { m_titles.value(object), static_cast<Contact*>(object) };
	int index = qBinaryFind(m_contacts, item) - m_contacts.constBegin();
	QModelIndex modelIndex = createIndex(index, 0, &m_contacts[index]);
	emit dataChanged(modelIndex, modelIndex);
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
}
}
