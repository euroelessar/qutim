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

#include "chatchannelmodel.h"

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;
enum {
	ChannelRole = Qt::UserRole,
	UnreadCountRole
};

ChatChannelModel::ChatChannelModel(QObject *parent) :
    QAbstractListModel(parent)
{
	QHash<int, QByteArray> roleNames;
	roleNames.insert(ChannelRole, "channel");
	roleNames.insert(UnreadCountRole, "unreadCount");
	roleNames.insert(Qt::DisplayRole, "title");
	roleNames.insert(Qt::DecorationRole, "iconSource");
	setRoleNames(roleNames);
	
	ChatLayer *chatLayer = ChatLayer::instance();
	connect(chatLayer, SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
	        SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	foreach (ChatSession *session, chatLayer->sessions())
		onSessionCreated(session);
}

int ChatChannelModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_sessions.size();
}

QVariant ChatChannelModel::data(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() > m_sessions.size())
		return QVariant();
	ChatSession *session = m_sessions[index.row()];
	switch (role) {
	case Qt::DisplayRole:
		return session->unit()->title();
	case Qt::DecorationRole:
		return QString();
	case ChannelRole:
		return qVariantFromValue<QObject*>(session);
	case UnreadCountRole:
		return session->unread().count();
	default:
		return QVariant();
	}
}

void ChatChannelModel::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	connect(session, SIGNAL(destroyed(QObject*)),
	        SLOT(onSessionDeath(QObject*)));
	beginInsertRows(QModelIndex(), m_sessions.size(), m_sessions.size());
	m_sessions << session;
	endInsertRows();
}

void ChatChannelModel::onSessionUnreadChanged()
{
	ChatSession *session = static_cast<ChatSession*>(sender());
	int index = m_sessions.indexOf(session);
	QModelIndex modelIndex = createIndex(index, 0, session);
	emit dataChanged(modelIndex, modelIndex);
}

void ChatChannelModel::onSessionDeath(QObject *object)
{
	int index = m_sessions.indexOf(static_cast<ChatSession*>(object));
	beginRemoveRows(QModelIndex(), index, index);
	m_sessions.removeAt(index);
	endRemoveRows();
}
}

