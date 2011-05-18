/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
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
