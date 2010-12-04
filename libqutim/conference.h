#ifndef CONFERENCE_H
#define CONFERENCE_H

#include "chatunit.h"
#include "buddy.h"

namespace qutim_sdk_0_3
{
class ConferencePrivate;
class Buddy;

class LIBQUTIM_EXPORT Conference : public ChatUnit
{
	Q_DECLARE_PRIVATE(Conference)
	Q_PROPERTY(QString topic READ topic WRITE setTopic NOTIFY topicChanged)
	Q_PROPERTY(qutim_sdk_0_3::Buddy* me READ me NOTIFY meChanged)
	Q_OBJECT
public:
	Conference(Account *account);
	virtual ~Conference();
	virtual QString topic() const;
	virtual void setTopic(const QString &topic);
	virtual Buddy *me() const = 0;
public slots:
	virtual void join() = 0;
	virtual void leave() = 0;
	virtual void invite(qutim_sdk_0_3::Contact *contact, const QString &reason = QString());
signals:
	void topicChanged(const QString &current, const QString &previous);
	void meChanged(qutim_sdk_0_3::Buddy *me);
	void left();
	void joined();
protected:
	bool event(QEvent *ev);
};

//small draft

//events
//groupchat-bookmark-list QList<AbstractBookMarksItem>
//groupchat-bookmark-fields AbstractBookMarksItem
//groupchat-bookmark-add AbstractBookMarksItem
//groupchat-bookmark-remove AbstractBookMarksItem
//groupchat-bookmark-update AbstractBookMarksItem
//class DataItem;
//class AbstractBookmarksItemPrivate;
//class AbstractBookmarksItem
//{
//public:
//	enum Type
//	{
//		Recent,
//		SavedLocal,
//		SavedOnServer
//	};
//	/** Constructs a new AbstractBookmarksItem with the given \a name
//	*/
//	AbstractBookmarksItem(const QString &name = QString(), Type = SavedLocal);
//	/** Returns type of item
//	*/
//	Type type();
//	/** Set type of item
//	*/
//	void setType();
//	/** Returns a DataItem with filled fields
//	*/
//	virtual DataItem *fields() = 0;
//	/** Set item name
//	*/
//	QString setName();
//	/** Returns human-readable item name
//	*/
//	QString name();
//	/** Returns description TODO
//	*/
//	QVariant description();
//	/**
//	*/
//	virtual ~AbstractBookmarksItem() {}
//};

//typedef QSharedPointer<AbstractBookmarksItem> AbstractBookmarksItemPointer;
//typedef QList<AbstractBookmarksItemPointer> BookmarksItemList;

class DataItem;
class GroupChatManagerPrivate;
class GroupChatManager
{
	Q_DECLARE_PRIVATE(GroupChatManager)
public:
	/** Constructs a new GroupChatManager with the given \a account.
	*/
	GroupChatManager(Account *account);
	/**
	*/
	virtual ~GroupChatManager();
	/** Returns the account this GroupChatManager is for.
	*/
	Account *account() const;
	/** Returns the dataitem fields
	*/
	virtual DataItem fields() const = 0;
	/** Join a groupchat with \a filled fields
	*/
	virtual void join(const DataItem &item) = 0;
	/** Save item, \note if an item with that name exists, it must update its
	*/
	virtual void save(const DataItem &item) = 0;
	/** Remove item
	*/
	virtual void remove(const DataItem &item) = 0;
	/** Returns a list of stored bookmarks
	*/
	virtual QList<DataItem> bookmarks() const = 0;
private:
	QScopedPointer<GroupChatManager> d_ptr;
};

}

Q_DECLARE_INTERFACE(qutim_sdk_0_3::GroupChatManager, "org.qutim.core.GroupChatManager");
Q_DECLARE_METATYPE(qutim_sdk_0_3::Conference*)
Q_DECLARE_METATYPE(QList<qutim_sdk_0_3::Conference*>)

#endif // CONFERENCE_H
