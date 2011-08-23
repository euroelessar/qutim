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

#include "chatmessagemodel.h"
#include <QUrl>
#include <qutim/chatunit.h>
#include <qutim/chatsession.h>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;
enum {
	TimeRole = Qt::UserRole,
	IncomingRole,
	UnitRole,
	IdRole,
	AvatarRole,
	TitleRole
};

ChatMessageModel::ChatMessageModel(QObject *parent) :
    QAbstractListModel(parent)
{
	QHash<int, QByteArray> roleNames;
	roleNames.insert(IdRole, "messageId");
	roleNames.insert(AvatarRole, "avatar");
	roleNames.insert(TitleRole, "title");
	roleNames.insert(Qt::DisplayRole, "text");
	roleNames.insert(Qt::DecorationRole, "iconSource");
	roleNames.insert(TimeRole, "time");
	roleNames.insert(IncomingRole, "incoming");
	roleNames.insert(UnitRole, "contact");
	setRoleNames(roleNames);
}

void ChatMessageModel::append(qutim_sdk_0_3::Message &msg)
{
	beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
	m_messages << msg;
	endInsertRows();
}

int ChatMessageModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_messages.size();
}

QVariant ChatMessageModel::data(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() >= m_messages.size())
		return QVariant();
	const Message &msg = m_messages[index.row()];
	switch (role) {
	case IdRole:
		return msg.id();
	case TitleRole:
	case Qt::DisplayRole:
		return msg.text();
	case AvatarRole:
	case Qt::DecorationRole: {
		QString avatar = msg.chatUnit()->property("avatar").toString();
		if (!avatar.isEmpty())
			return QUrl::fromLocalFile(avatar);
		return QString();
	}
	case TimeRole:
		return msg.time();
	case UnitRole:
		return qVariantFromValue<QObject*>(msg.chatUnit());
	default:
		return QVariant();
	}
}
}
