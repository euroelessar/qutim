#ifndef GROUPCHATMANAGER_H
#define GROUPCHATMANAGER_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{

class DataItem;
class Protocol;
class Account;
class GroupChatManagerPrivate;

class LIBQUTIM_EXPORT GroupChatManager
{
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

Q_DECLARE_INTERFACE(qutim_sdk_0_3::GroupChatManager, "org.qutim.core.GroupChatManager");

#endif // GROUPCHATMANAGER_H
