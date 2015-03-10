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
#ifndef GROUPCHATMANAGER_H
#define GROUPCHATMANAGER_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{

class DataItem;
class Protocol;
class Account;
class GroupChatManagerPrivate;

class LIBQUTIM_EXPORT GroupChatManager : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Interface", "GroupChatManager")
public:
	/**
		Constructs a new GroupChatManager with the given \a account.
	*/
	GroupChatManager(Account *account);
	/**
		Destroyes this GroupChatManager.
	*/
	virtual ~GroupChatManager();
	/**
		Returns the account this GroupChatManager is for.
	*/
	Account *account() const;
	/**
		Returns the dataitem fields.
	*/
	virtual DataItem fields() const = 0;
	/**
		Join a groupchat with filled fields.
	*/
	virtual bool join(const DataItem &fields) = 0;
	/**
		Store bookmark.

		Returns true if the bookmark has been successfully stored; otherwise returns false.
	*/
	virtual bool storeBookmark(const DataItem &fields, const DataItem &oldFields) = 0;
	/**
		Remove bookmark.

		Returns true if the bookmark has been successfully removed; otherwise returns false.
	*/
	virtual bool removeBookmark(const DataItem &fields) = 0;
	/**
		Returns a list of stored bookmarks.
	*/
	virtual QList<DataItem> bookmarks() const = 0;
	/**
		Returns a list of group chats that were joined recently.
	*/
	virtual QList<DataItem> recent() const = 0;
	/**
		Returns the list of all managers.

		\see Account::getGroupChatManager()
	*/
	static QList<GroupChatManager*> allManagers();
private:
	QScopedPointer<GroupChatManagerPrivate> d;
};

}

#endif // GROUPCHATMANAGER_H

