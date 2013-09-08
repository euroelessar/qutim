/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2013 Roman Tretyakov <roman@trett.ru>
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

#ifndef CHATSESSIONMODEL_H
#define CHATSESSIONMODEL_H

#include <QAbstractListModel>
#include "chatsessionimpl.h"

enum ContactItemRole
{
	BuddyRole = Qt::UserRole,
	StatusRole,
	ContactsCountRole,
	OnlineContactsCountRole,
	AvatarRole,
	ItemTypeRole
};
Q_DECLARE_FLAGS(ContactItemRoles,ContactItemRole);
enum ContactItemType
{
	InvalidType = 0,
	TagType = 100,
	ContactType = 101
};

namespace Core
{
namespace AdiumChat
{
using namespace qutim_sdk_0_3;

class ChatSessionModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit ChatSessionModel(ChatSessionImpl *parent = 0);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	void addContact(qutim_sdk_0_3::Buddy *c);
	void removeContact(qutim_sdk_0_3::Buddy *c);
private slots:
	void onNameChanged(const QString &title, const QString &oldTitle);
	void onStatusChanged(const qutim_sdk_0_3::Status &status);
	void onContactDestroyed(QObject *obj);
	void onPriorityChanged();
private:
	struct Node {
		Node(qutim_sdk_0_3::Buddy *u, const QString &t) : title(t), unit(u) {}
		Node(qutim_sdk_0_3::Buddy *u) : title(u->title()), unit(u) {}
		Node() : unit(NULL) {}
		QString title;
		qutim_sdk_0_3::Buddy *unit;
		int priority = unit->property("priority").toInt();
		
		bool operator <(const Node &o) const
		{
			int cmp = o.priority - priority;
			if (cmp == 0)
				cmp = title.compare(o.title, Qt::CaseInsensitive);
			return cmp < 0 || (cmp == 0 && unit < o.unit);
		}
	};

	QList<Node> m_units;
};
}
}
#endif // CHATSESSIONMODEL_H

