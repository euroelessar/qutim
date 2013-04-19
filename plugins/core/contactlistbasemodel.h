/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef CONTACTLISTMODELBASE_H
#define CONTACTLISTMODELBASE_H

#include "account.h"
#include "contact.h"
#include <QAbstractItemModel>
#include <QBasicTimer>
#include <QStringList>
#include <QPointer>

class ContactListFrontModel;

enum ContactListItemRole
{
	StatusRole = Qt::UserRole,
	ContactsCountRole,
	OnlineContactsCountRole,
	AvatarRole,
	ItemTypeRole,
	AccountRole,
	ColorRole,
	TagNameRole,
	NotificationRole,
	StatusTextRole,
	StatusIconNameRole,
    StatusTypeRole,
    StatusIdRole,
	ContactRole,
	AlphabetRole,
	IdRole
};

enum ContactListItemType
{
	InvalidType = 0,
	TagType = 100,
	ContactType = 101,
	AccountType = 102
};

class ContactListBaseModel : public QAbstractItemModel
{
    Q_OBJECT
	Q_PROPERTY(QStringList tags READ tags NOTIFY tagsChanged)
public:
    explicit ContactListBaseModel(QObject *parent = 0);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	QStringList tags() const;

	virtual void updateContactTags(Contact *contact,
								   const QStringList &current,
								   const QStringList &previous);
	virtual void addAccount(Account *account);
	virtual void removeAccount(Account *account);
	virtual void addContact(Contact *contact) = 0;
	virtual void removeContact(Contact *contact) = 0;

private:
	class Comparator;

protected:
	class ContactNode;
	class AccountNode;
	class ContactListNode;
	class TagListNode;
	class AccountListNode;

	enum NodeType
	{
		ContactListNodeType     = 0x01,
		TagListNodeType         = 0x02 | ContactListNodeType,
		AccountListNodeType     = 0x04 | TagListNodeType,
		ProtocolListNodeType    = 0x08 | AccountListNodeType,
		ContactNodeType         = 0x10,
		TagNodeType             = 0x20 | ContactListNodeType,
		AccountNodeType         = 0x40 | TagListNodeType,
		RootNodeType            = 0x80 | AccountListNodeType
	};

	template <typename T>
	class Pointer
	{
	public:
		explicit Pointer(T *data) : m_guard(data), m_data(data) {}

		T *data() const { Q_ASSERT(m_guard); return m_guard.data(); }
		bool operator! () const { return !m_guard; }
		bool operator== (T *other) const { return m_data == other; }
		bool operator!= (T *other) const { return m_data != other; }

	private:
		friend class Comparator;
		friend class AccountNode;
		friend class ContactNode;

		QPointer<T> m_guard;
		T *m_data;
	};

    class BaseNode
	{
    public:
		inline BaseNode(NodeType type, BaseNode *parent) : m_type(type), m_parent(parent) {}

		inline NodeType type() const { return m_type; }
		inline BaseNode *parent() const { return m_parent; }

	private:
		NodeType m_type;
		BaseNode *m_parent;
	};

	class ContactNode : public BaseNode
	{
	public:
		enum { Type = ContactNodeType };

		inline ContactNode(Contact *contact, BaseNode &parent)
			: BaseNode(ContactNodeType, &parent), contact(contact) {}

		inline ContactListNode *parent() const { return static_cast<ContactListNode*>(BaseNode::parent()); }
		inline bool operator  <(const ContactNode &other) const { return contact.m_data  < other.contact.m_data; }
		inline bool operator ==(const ContactNode &other) const { return contact.m_data == other.contact.m_data; }

		Pointer<Contact> contact;
	};

	class ContactListNode : public BaseNode
	{
	public:
		enum { Type = ContactListNodeType };

		inline ContactListNode(NodeType type, BaseNode *parent) : BaseNode(type, parent) {}

		QList<ContactNode> contacts;
		QHash<Contact*, int> onlineContacts;
		QHash<Contact*, int> totalContacts;
	};

	class TagNode : public ContactListNode
	{
	public:
		enum { Type = TagNodeType };

		explicit inline TagNode(const QString &name, BaseNode &parent)
			: ContactListNode(TagNodeType, &parent), name(name) {}

		inline TagListNode *parent() const { return static_cast<TagListNode*>(BaseNode::parent()); }
		inline bool operator  <(const TagNode &other) const { return name  < other.name; }
		inline bool operator ==(const TagNode &other) const { return name == other.name; }

		QString name;
	};

	class TagListNode : public ContactListNode
	{
	public:
		enum { Type = TagListNodeType };

		inline TagListNode(NodeType type, BaseNode *parent) : ContactListNode(type, parent) {}

		QList<TagNode> tags;
	};

	class AccountNode : public TagListNode
    {
    public:
		enum { Type = AccountNodeType };

		explicit inline AccountNode(Account *account, BaseNode &parent)
			: TagListNode(AccountNodeType, &parent), account(account) {}

		inline AccountListNode *parent() const { return static_cast<AccountListNode*>(BaseNode::parent()); }

		inline bool operator  <(const AccountNode &other) const { return account.m_data  < other.account.m_data; }
		inline bool operator ==(const AccountNode &other) const { return account.m_data == other.account.m_data; }

		Pointer<Account> account;
	};

	class AccountListNode : public TagListNode
	{
	public:
		enum { Type = AccountListNodeType };

		inline AccountListNode(NodeType type, BaseNode *parent) : TagListNode(type, parent) {}

		QList<AccountNode> accounts;
	};

	class RootNode : public AccountListNode
	{
	public:
		enum { Type = RootNodeType };

		inline RootNode() : AccountListNode(RootNodeType, NULL) {}
	};

	AccountNode *ensureAccount(Account *account, AccountListNode *parent);
	void eraseAccount(Account *account, AccountListNode *parent);
	TagNode *ensureTag(const QString &name, TagListNode *parent);
	ContactNode *ensureContact(Contact *contact, ContactListNode *parent);
	void eraseContact(Contact *contact, ContactListNode *parent);

	RootNode *rootNode() const;
	QStringList emptyTags() const;
	QStringList fixTags(const QStringList &tags) const;

signals:
	void tagsChanged(const QStringList &tags);

private slots:
	void onAccountCreated(Account *account, bool addContacts = true);
	void onAccountDestroyed(QObject *obj);
	void onAccountRemoved(Account *account);
	void onContactDestroyed(QObject *obj);
    void onContactsChanged(const QList<Contact*> &contactsAdded,
                           const QList<Contact*> &contactsRemoved);
	void onContactAdded(Contact *contact);
	void onContactRemoved(Contact *contact);
	void onContactChanged(Contact *contact, bool parentsChanged = false);
	void onContactChanged();
	void onContactTagsChanged(const QStringList &current, const QStringList &previous);
	void onStatusChanged(const Presence &current, const Presence &previous);

	void connectContact(Contact *contact);
	void disconnectContact(Contact *contact);

private:
	class Comparator
	{
	public:
		inline bool operator() (const AccountNode &first, Account *second) const
		{ return first.account.m_data < second; }
		inline bool operator() (Account *first, const AccountNode &second) const
		{ return first < second.account.m_data; }
		inline bool operator() (const TagNode &first, const QString &second) const
		{ return first.name < second; }
		inline bool operator() (const QString &first, const TagNode &second) const
		{ return first < second.name; }
		inline bool operator() (const ContactNode &first, Contact *second) const
		{ return first.contact.m_data < second; }
		inline bool operator() (Contact *first, const ContactNode &second) const
		{ return first < second.contact.m_data; }
	};

	bool findNode(BaseNode *node) const;
	bool findNode(BaseNode *node, BaseNode *current) const;

	void findContacts(QSet<Contact*> &contacts, BaseNode *current);
	void addTags(const QStringList &tags);

	void updateItemCount(Contact *contact, ContactListNode *parent, int online, int total);
	void removeAccountNode(Account *account, BaseNode *parent);
	void clearContacts(BaseNode *parent);

	QModelIndex createIndex(BaseNode *node) const;
	inline QModelIndex createIndex(BaseNode &node, int row) const
	{ return QAbstractItemModel::createIndex(row, 0, &node); }
	BaseNode *extractNode(const QModelIndex &index) const;
	template <typename T> static inline T node_cast(BaseNode *node)
	{ return (node && (node->type() & reinterpret_cast<T>(NULL)->Type) == reinterpret_cast<T>(NULL)->Type) ? static_cast<T>(node) : NULL; }
	template <typename T> inline T *extractNode(const QModelIndex &index) const
	{ return node_cast<T*>(extractNode(index)); }

	typedef QHash<Contact*, QList<ContactNode *> > ContactHash;
	friend class ContactListFrontModel;

	RootNode m_root;
	ContactHash m_contactHash;
	mutable QStringList m_emptyTags;
	QStringList m_tags;
};

#endif // CONTACTLISTMODELBASE_H
