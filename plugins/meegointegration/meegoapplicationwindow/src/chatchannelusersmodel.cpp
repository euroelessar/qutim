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

#include "chatchannelusersmodel.h"
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

ChatChannelUsersModel::ChatChannelUsersModel(QObject *parent) :
    QAbstractListModel(parent)
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
	
	m_statusPrefix = QLatin1String("icon-m-common");
}

QString ChatChannelUsersModel::statusPrefix()
{
	return m_statusPrefix;
}

void ChatChannelUsersModel::setStatusPrefix(const QString &prefix)
{
	if (m_statusPrefix == prefix)
		return;
	m_statusPrefix = prefix;
	emit statusPrefixChanged(m_statusPrefix);
}

void ChatChannelUsersModel::addUnit(Buddy *unit)
{
	const Node node(unit);
	const QList<Node>::Iterator it = qLowerBound(m_units.begin(), m_units.end(), node);
	if (it != m_units.end() && it->unit == unit)
		return;
	int index = it - m_units.begin();
	beginInsertRows(QModelIndex(), index, index);
	m_units.insert(index, unit);
	connect(unit, SIGNAL(destroyed(QObject*)),
	        this, SLOT(onUnitDeath(QObject*)));
	connect(unit, SIGNAL(titleChanged(QString,QString)),
	        this, SLOT(onTitleChanged(QString,QString)));
	connect(unit, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
	endInsertRows();
	emit countChanged(m_units.size());
}

void ChatChannelUsersModel::removeUnit(Buddy *unit)
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
	emit countChanged(m_units.size());
}

void ChatChannelUsersModel::onUnitDeath(QObject *object)
{
	for (int i = 0; i < m_units.size(); ++i) {
		if (m_units.at(i).unit == object) {
			beginRemoveRows(QModelIndex(), i, i);
			m_units.removeAt(i);
			endRemoveRows();
			emit countChanged(m_units.size());
			return;
		}
	}
}

void ChatChannelUsersModel::onStatusChanged(const qutim_sdk_0_3::Status &)
{
	Buddy *unit = static_cast<Buddy*>(sender());
	Q_ASSERT(unit);
	const Node node(unit);
	const QList<Node>::Iterator it = qBinaryFind(m_units.begin(), m_units.end(), node);
	if (it == m_units.end())
		return;
	int index = it - m_units.begin();
	dataChanged(createIndex(index, 0), createIndex(index, 0));
}

void ChatChannelUsersModel::onTitleChanged(const QString &title, const QString &oldTitle)
{
	Buddy *unit = static_cast<Buddy*>(sender());
	QList<Node>::Iterator it;
	it = qBinaryFind(m_units.begin(), m_units.end(), Node(unit, oldTitle));
	Q_ASSERT(it != m_units.end());
	const int from = it - m_units.begin();
	it = qLowerBound(m_units.begin(), m_units.end(), Node(unit, title));
	int to = it - m_units.begin();
	m_units[from].title = title;
	if (beginMoveRows(QModelIndex(), from, from, QModelIndex(), to)) {
		if (to > from)
			--to;
		m_units.move(from, to);
		Q_ASSERT(m_units[to].unit == unit);
		Q_ASSERT(m_units[to].unit->title() == title);
		endMoveRows();
	}
}

int ChatChannelUsersModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_units.size();
}

QVariant ChatChannelUsersModel::data(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() >= m_units.size())
		return QVariant();
	Buddy *unit = m_units[index.row()].unit;
	switch (role) {
	case IdRole:
		return unit->id();
	case Qt::DisplayRole:
		return unit->title();
	case Qt::DecorationRole:
		return ContactList::statusUrl(unit->status().type(), m_statusPrefix);
	case ContactRole:
		return qVariantFromValue<QObject*>(unit);
	case AlphabetRole:
		return unit->title().at(0).toUpper();
	case StatusTextRole:
		return unit->status().text();
	case AvatarRole:
		return unit->avatar();
	default:
		return QVariant();
	}
}
}

