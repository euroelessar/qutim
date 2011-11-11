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

#include "chatsessionmodel.h"

namespace Core
{
namespace AdiumChat
{
ChatSessionModel::ChatSessionModel(ChatSessionImpl *parent) :
	QAbstractListModel(parent)
{
}

int ChatSessionModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : m_units.count();
}

QVariant ChatSessionModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.column() >= 1)
		return QVariant();
	Buddy *buddy = m_units.value(index.row());
	if (!buddy)
		return QVariant();
	switch (role) {
	case Qt::DisplayRole:
		return buddy->title();
	case Qt::DecorationRole:
		return buddy->status().icon();
	case BuddyRole:
		return qVariantFromValue(buddy);
	case AvatarRole:
		return buddy->avatar();
	case StatusRole:
		return qVariantFromValue(buddy->status());
	case ItemTypeRole:
		return ContactType;
	default:
		return QVariant();
	}
}

bool contactLessThan(Buddy *a, Buddy *b)
{
	//	if (a->status.type() < b->status.type() )
	//		return true;
	//	else if (a->status.type() > b->status.type())
	//		return false;
	return a->title().compare(b->title(), Qt::CaseInsensitive) < 0;
};

void ChatSessionModel::addContact(Buddy *b)
{
	if (m_units.contains(b))
		return;
	int index = qLowerBound(m_units.constBegin(), m_units.constEnd(),
							b, contactLessThan) - m_units.constBegin();
	connect(b, SIGNAL(titleChanged(QString,QString)), this, SLOT(onNameChanged(QString)));
	connect(b, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
	connect(b, SIGNAL(destroyed(QObject*)), this, SLOT(onContactDestroyed(QObject*)));
	beginInsertRows(QModelIndex(), index, index);
	m_units.insert(index, b);
	endInsertRows();
}

void ChatSessionModel::removeContact(Buddy *b)
{
	int index = m_units.indexOf(b);
	if (index < 0)
		return;
	beginRemoveRows(QModelIndex(), index, index);
	m_units.removeAt(index);
	endRemoveRows();
	disconnect(b, 0, this, 0);
}

void ChatSessionModel::onNameChanged(const QString &)
{
	Buddy *b = qobject_cast<Buddy*>(sender());
	Q_ASSERT(b);
	int index = m_units.indexOf(b);
	int newIndex = qLowerBound(m_units.constBegin(), m_units.constEnd(),
							   b, contactLessThan) - m_units.constBegin();
	beginMoveRows(QModelIndex(), index, index, QModelIndex(), newIndex);
	m_units.move(index, newIndex);
	endMoveRows();
	dataChanged(createIndex(newIndex, 0, b), createIndex(newIndex, 0, b));
}

void ChatSessionModel::onStatusChanged(const qutim_sdk_0_3::Status &)
{
	Buddy *b = qobject_cast<Buddy*>(sender());
	Q_ASSERT(b);
	int index = m_units.indexOf(b);
	dataChanged(createIndex(index, 0, b), createIndex(index, 0, b));
}


void ChatSessionModel::onContactDestroyed(QObject *obj)
{
	removeContact(reinterpret_cast<Buddy*>(obj));
}

}
}

