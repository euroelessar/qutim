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
#include <qutim/account.h>
#include <qutim/chatunit.h>
#include <qutim/chatsession.h>
#include <qutim/conference.h>
#include <QDateTime>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;
enum {
	TimeRole = Qt::UserRole,
	IncomingRole,
	UnitRole,
	IdRole,
	SenderAvatarRole,
	AccountAvatarRole,
	TitleRole,
	SenderNameRole,
	DeliveredRole,
	HtmlRole,
	ActionRole,
	ServiceRole,
	AppendingRole
};

ChatMessageModel::ChatMessageModel(QObject *parent) :
    QAbstractListModel(parent)
{
	QHash<int, QByteArray> roleNames;
	roleNames.insert(IdRole, "messageId");
	roleNames.insert(SenderNameRole, "senderName");
	roleNames.insert(SenderAvatarRole, "senderAvatar");
	roleNames.insert(AccountAvatarRole, "accountAvatar");
	roleNames.insert(TitleRole, "title");
	roleNames.insert(Qt::DisplayRole, "text");
	roleNames.insert(Qt::DecorationRole, "iconSource");
	roleNames.insert(TimeRole, "time");
	roleNames.insert(IncomingRole, "incoming");
	roleNames.insert(UnitRole, "contact");
	roleNames.insert(DeliveredRole, "delivered");
	roleNames.insert(HtmlRole, "html");
	roleNames.insert(ActionRole, "action");
	roleNames.insert(ServiceRole, "service");
	roleNames.insert(AppendingRole, "appending");
	setRoleNames(roleNames);
	parent->installEventFilter(this);
}

void ChatMessageModel::append(qutim_sdk_0_3::Message &msg)
{
	beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
	m_messages << msg;
	endInsertRows();
}

bool ChatMessageModel::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == MessageReceiptEvent::eventType()) {
		MessageReceiptEvent *event = static_cast<MessageReceiptEvent*>(ev);
	}
	return QAbstractListModel::eventFilter(obj, ev);
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
	case AccountAvatarRole: {
		QString avatar = msg.chatUnit()->account()->property("avatar").toString();
		if (!avatar.isEmpty())
			return QUrl::fromLocalFile(avatar);
		return QString();
	}
	case SenderAvatarRole:
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
	case SenderNameRole:
		return createSenderName(msg);
	case DeliveredRole:
		return false;
	case HtmlRole:
		return msg.property("html");
	case ActionRole:
		return msg.property("action", false);
	case ServiceRole:
		return msg.property("service", false);
	case AppendingRole:
		if (index.row() > 0) {
			const Message &prev = m_messages[index.row() - 1];
			return prev.isIncoming() == msg.isIncoming()
			        && prev.time().date() == msg.time().date()
			        && createSenderName(prev) == createSenderName(msg);
		}
		return false;
	default:
		return QVariant();
	}
}

QString ChatMessageModel::createSenderName(const Message &msg) const
{
	QString senderName = msg.property("senderName",QString());
	if (senderName.isEmpty()) {
		if (!msg.isIncoming()) {
			const Conference *conf = qobject_cast<const Conference*>(msg.chatUnit());
			if (conf && conf->me())
				senderName = conf->me()->title();
			else
				senderName = msg.chatUnit()->account()->name();
		} else {
			senderName = msg.chatUnit()->title();
		}
	}
	return senderName;
}
}
