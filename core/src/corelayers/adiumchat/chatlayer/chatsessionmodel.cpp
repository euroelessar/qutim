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
	Buddy *buddy = m_units.at(index.row()).unit;
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

void ChatSessionModel::addContact(Buddy *unit)
{
	const Node node(unit);
	const QList<Node>::Iterator it = qLowerBound(m_units.begin(), m_units.end(), node);
	if (it != m_units.end() && it->unit == unit)
		return;
	int index = it - m_units.begin();
	beginInsertRows(QModelIndex(), index, index);
	m_units.insert(index, unit);
	connect(unit, SIGNAL(titleChanged(QString,QString)),
	        this, SLOT(onNameChanged(QString,QString)));
	connect(unit, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
	connect(unit, SIGNAL(destroyed(QObject*)),
	        this, SLOT(onContactDestroyed(QObject*)));
	endInsertRows();
}

void ChatSessionModel::removeContact(Buddy *unit)
{
	const Node node(unit);
	const QList<Node>::Iterator it = qBinaryFind(m_units.begin(), m_units.end(), node);
	if (it == m_units.end())
		return;
	int index = it - m_units.begin();
	beginRemoveRows(QModelIndex(), index, index);
	disconnect(unit, 0, this, 0);
	m_units.removeAt(index);
	endRemoveRows();
}

void ChatSessionModel::onNameChanged(const QString &title, const QString &oldTitle)
{
	Buddy *unit = static_cast<Buddy*>(sender());
	QList<Node>::Iterator it;
	it = qBinaryFind(m_units.begin(), m_units.end(), Node(unit, oldTitle));
	const int from = it - m_units.begin();
	it = qLowerBound(m_units.begin(), m_units.end(), Node(unit, title));
	int to = it - m_units.begin();
	if (to > from)
		--to;
	to = qMin(to, m_units.size() - 1);
	beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
	m_units.move(from, to);
	m_units[to].title = title;
	endMoveRows();
}

void ChatSessionModel::onStatusChanged(const qutim_sdk_0_3::Status &)
{
	Buddy *unit = qobject_cast<Buddy*>(sender());
	Q_ASSERT(unit);
	Node node(unit);
	const QList<Node>::Iterator it = qBinaryFind(m_units.begin(), m_units.end(), node);
	if (it == m_units.end())
		return;
	int index = it - m_units.begin();
	dataChanged(createIndex(index, 0, unit), createIndex(index, 0, unit));
}


void ChatSessionModel::onContactDestroyed(QObject *object)
{
	for (int i = 0; i < m_units.size(); ++i) {
		if (m_units.at(i).unit == object) {
			beginRemoveRows(QModelIndex(), i, i);
			m_units.removeAt(i);
			endRemoveRows();
			return;
		}
	}
}

}
}

