/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "floatiesitemmodel.h"

FloatiesItemModel::FloatiesItemModel(QObject *parent) : QAbstractListModel(parent)
{
}

FloatiesItemModel::~FloatiesItemModel()
{
}

int FloatiesItemModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_contacts.size();
}

QVariant FloatiesItemModel::data(const QModelIndex &index, int role) const
{
	qutim_sdk_0_3::Contact *contact = m_contacts.at(index.row());
	switch(role)
	{
	case Qt::EditRole:
	case Qt::DisplayRole: {
			QString name = contact->name();
			return name.isEmpty() ? contact->id() : name;
		}
	case Qt::DecorationRole:
//		if (p->showMessageIcon && p->unreadContacts.contains(contact))
//			return p->unreadIcon;
//		else
			return contact->status().icon();
	case ItemTypeRole:
		return ContactType;
	case StatusRole:
		return qVariantFromValue(contact->status());
	case AvatarRole:
		return contact->avatar();
	default:
		return QVariant();
	}
}

QPersistentModelIndex FloatiesItemModel::addContact(qutim_sdk_0_3::Contact *contact)
{
	beginInsertRows(QModelIndex(), m_contacts.size(), m_contacts.size());
	m_contacts.append(contact);
	endInsertRows();
	QModelIndex modelIndex = index(m_contacts.size() -1, 0, QModelIndex());
	return QPersistentModelIndex(modelIndex);
}

void FloatiesItemModel::removeContact(qutim_sdk_0_3::Contact *contact)
{
	int index = m_contacts.indexOf(contact);
	if (index == -1)
		return;
	beginRemoveRows(QModelIndex(), index, index);
	m_contacts.removeAt(index);
	endRemoveRows();
}

