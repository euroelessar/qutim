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
#ifndef SIMPLECONTACTLISTMODEL_H
#define SIMPLECONTACTLISTMODEL_H

#include <QAbstractItemModel>
#include <qutim/simplecontactlist/simplecontactlistitem.h>
#include <qutim/chatsession.h>
#include <qutim/simplecontactlist/abstractcontactmodel.h>

namespace Core
{
namespace SimpleContactList
{
class SeparatedModelPrivate;
class SeparatedModel;

class ChangeEvent;
class ContactItem;
class TagItem;

class AccountItem : public ItemHelper
{
public:
	inline AccountItem() : ItemHelper(AccountType) {}
	Account *account;
	QString id;
	QList<TagItem*> tags;
	QList<TagItem *> visibleTags;
	QHash<QString, TagItem *> tagsHash;
};

class TagItem : public ItemHelper
{
public:
	inline TagItem() : ItemHelper(TagType), online(0) {}
	inline AccountItem *getTagContainer(void*) { return parent; }
	inline void setTagContainer(AccountItem *p) { parent = p; }
	inline QModelIndex parentIndex(AbstractContactModel *m);
	QString getName();
	QList<ContactItem *> visible;
	int online;
	QString name;
	QList<ContactItem *> contacts;
	AccountItem *parent;
};

class ContactData : public QSharedData
{
public:
	typedef QExplicitlySharedDataPointer<ContactData> Ptr;
	inline ContactData() {}
	inline ContactData(const ContactData &other)
		: QSharedData(other), contact(other.contact), tags(other.tags), items(other.items) {}
	inline ~ContactData() {}
	QWeakPointer<Contact> contact;
	QSet<QString> tags;
	QList<ContactItem *> items;
	Status status;
};

class ContactItem  : public ItemHelper
{
public:
	inline ContactItem() : ItemHelper(ContactType) {}
	inline ContactItem(const ContactData::Ptr &other_data) : ItemHelper(ContactType), data(other_data) {}
	inline ContactItem(const ContactItem &other) : ItemHelper(ContactType), parent(other.parent), data(other.data) {}
	inline int index() { return parent->visible.indexOf(this); }
	inline QModelIndex parentIndex(AbstractContactModel *m);
	inline Contact *getContact() { return data->contact.data(); }
	inline Status getStatus() { return data->status; }
	inline QList<ContactItem*> &siblings(void*) { return parent->visible; }
	inline bool isInSelectedTag(const QSet<QString> &selectedTags) { return selectedTags.contains(parent->name); }
	TagItem *parent;
	ContactData::Ptr data;
};

class SeparatedModel : public Core::SimpleContactList::AbstractContactModel
{
	Q_OBJECT
	Q_INTERFACES(Core::SimpleContactList::AbstractContactModel)
	Q_DECLARE_PRIVATE(SeparatedModel)
	Q_CLASSINFO("SettingsDescription", "Show accounts, tags and contacts")
public:
	SeparatedModel(QObject *parent = 0);
	virtual ~SeparatedModel();
	virtual QList<Contact*> contacts() const;
	virtual void setContacts(const QList<qutim_sdk_0_3::Contact*> &contacts);
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	bool containsContact(Contact *contact) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	QStringList mimeTypes() const;
	QMimeData *mimeData(const QModelIndexList &indexes) const;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action,
					  int row, int column, const QModelIndex &parent);
	QStringList tags() const;
public slots:
	void addContact(qutim_sdk_0_3::Contact *contact);
	void removeContact(qutim_sdk_0_3::Contact *contact);
protected slots:
	void contactDeleted(QObject *obj);
	void contactTagsChanged(const QStringList &tags);
	void onContactInListChanged(bool isInList);
	AccountItem *onAccountCreated(qutim_sdk_0_3::Account *);
	void onAccountDestroyed(QObject *);
	void init();
protected:
	AccountItem *addAccount(qutim_sdk_0_3::Account *account, bool addContacts);
	void filterAllList();
	void updateContactData(Contact *contact);
	void processEvent(ChangeEvent *ev);
	bool eventFilter(QObject *obj, QEvent *ev);
	virtual void doContactChange(Contact *contact);
private:
	friend class TagItem;
	friend class ContactItem;
	void removeFromContactList(Contact *contact, bool deleted);
	void saveTagOrder(AccountItem *accountItem);
};

}
}

#endif // SIMPLECONTACTLISTMODEL_H

