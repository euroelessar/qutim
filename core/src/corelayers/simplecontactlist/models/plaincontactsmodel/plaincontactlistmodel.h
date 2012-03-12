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
#ifndef PLAINCONTACTLISTMODEL_H
#define PLAINCONTACTLISTMODEL_H

#include <QAbstractItemModel>
#include <simplecontactlistitem.h>
#include <qutim/chatsession.h>
#include <abstractcontactmodel.h>

namespace Core
{
namespace SimpleContactList
{
class PlainModelPrivate;
class PlainModel;

class ChangeEvent;
class ContactItem;

class ContactItem  : public ItemHelper
{
public:
	inline ContactItem() : ItemHelper(ContactType) {}
	inline ContactItem(const ContactItem &other) :
		ItemHelper(ContactType),
		contact(other.contact),
		tags(other.tags),
		status(other.status)
	{}
	inline QModelIndex parentIndex(void*) { return QModelIndex(); }
	inline Contact *getContact() { return contact.data(); }
	inline Status getStatus() { return status; }
	inline QList<ContactItem*> &siblings(AbstractContactModel *m);
	inline bool isInSelectedTag(QSet<QString> &selectedTags);
	QWeakPointer<Contact> contact;
	QSet<QString> tags;
	Status status;
};

class PlainModel : public Core::SimpleContactList::AbstractContactModel
{
	Q_OBJECT
	Q_INTERFACES(Core::SimpleContactList::AbstractContactModel)
	Q_DECLARE_PRIVATE(PlainModel)
	Q_CLASSINFO("SettingsDescription", "Show only contacts")
public:
	PlainModel(QObject *parent = 0);
	virtual ~PlainModel();
	virtual QList<Contact*> contacts() const;
	void setContacts(const QList<qutim_sdk_0_3::Contact*> &contacts);
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
	bool containsContact(Contact *contact) const;
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
	void contactStatusChanged(const qutim_sdk_0_3::Status &status);
	void contactNameChanged(const QString &name);
	void contactTagsChanged(const QStringList &tags);
	void onContactInListChanged(bool isInList);
	void onAccountCreated(qutim_sdk_0_3::Account *);
	void init();
protected:
	void filterAllList();
	void updateContactData(Contact *contact);
	bool changeContactVisibility(ContactItem *item, bool visibility);
	void processEvent(ChangeEvent *ev);
	bool eventFilter(QObject *obj, QEvent *ev);
private:
	friend class ContactItem;
	void removeFromContactList(Contact *contact, bool deleted);
};

}
}

#endif // PLAINCONTACTLISTMODEL_H

