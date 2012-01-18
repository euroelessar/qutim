/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef ABSTRACTCONTACTMODEL_H
#define ABSTRACTCONTACTMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include "simplecontactlist_global.h"
#include <qutim/status.h>
#include <qutim/message.h>
#include <qutim/notification.h>

namespace qutim_sdk_0_3
{
class Contact;
class Account;
class Buddy;
class ChatSession;
}

namespace Core {
namespace SimpleContactList {

class AbstractContactModelPrivate;
class ChangeEvent;
class ItemHelper;

class SIMPLECONTACTLIST_EXPORT AbstractContactModel : public QAbstractItemModel, public qutim_sdk_0_3::NotificationBackend
{
    Q_OBJECT
	Q_CLASSINFO("Service", "ContactModel")
	Q_CLASSINFO("RuntimeSwitch", "yes")
	Q_DECLARE_PRIVATE(AbstractContactModel)
public:
	virtual ~AbstractContactModel();
	QSet<QString> selectedTags() const;
	Q_INVOKABLE virtual QStringList tags() const;
	Q_INVOKABLE virtual QList<qutim_sdk_0_3::Contact*> contacts() const = 0;
	Q_INVOKABLE virtual void setContacts(const QList<qutim_sdk_0_3::Contact*> &contacts) = 0;
public slots:
	bool showOffline() const;
	void hideShowOffline();
	void filterList(const QString &name);
	void filterList(const QStringList &tags);
protected:
	AbstractContactModel(AbstractContactModelPrivate *d, QObject *parent = 0);
	template<typename TagContainer, typename TagItem, typename ContactItem>
	bool hideContact(ContactItem *item, bool hide, bool replacing = true);
	template<typename TagContainer, typename TagItem>
	void hideTag(TagItem *item);
	template<typename TagContainer, typename TagItem>
	void showTag(TagItem *item);
	template<typename TagItem, typename TagContainer>
	TagItem *ensureTag(TagContainer *p, const QString &name);
	template<typename ContactItem>
	void updateContact(ContactItem *item, bool placeChanged);
	template<typename ContactItem>
	QVariant contactData(const QModelIndex &index, int role) const;
	template<typename TagItem>
	QVariant tagData(const QModelIndex &index, int role) const;
	template<typename AccountItem>
	QVariant accountData(const QModelIndex &index, int role) const;
	template<typename TagContainer, typename TagItem,
			 typename ContactData, typename ContactItem>
	void updateContactStatus(typename ContactData::Ptr item_data, const qutim_sdk_0_3::Status &status);
	template<typename TagContainer, typename TagItem,
			 typename ContactData, typename ContactItem>
	void updateContactTags(TagContainer *p, typename ContactData::Ptr item_data, const QStringList &tags);
	template<typename ContactItem>
	bool isVisible(ContactItem *item);
	template<typename TagContainer, typename TagItem, typename ContactItem>
	void moveTag(ChangeEvent *ev);
	template<typename ContactItem>
	void showContactMergeDialog(ContactItem *parent, ContactItem *child);
protected:
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	int columnCount(const QModelIndex &) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	Qt::DropActions supportedDropActions() const;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action,
					  int row, int column, const QModelIndex &parent);
	void timerEvent(QTimerEvent *timerEvent);
	void handleNotification(qutim_sdk_0_3::Notification *notification);
	QIcon getIconForNotification(qutim_sdk_0_3::Notification *notification) const;
	static void setEncodedData(QMimeData *mimeData, const QString &type, const QModelIndex &index);
	static ItemHelper *decodeMimeData(const QMimeData *mimeData, const QString &type);
private slots:
	void init();
	void onNotificationFinished();
	void onContactDestroyed();
protected:
	virtual void filterAllList() = 0;
	virtual void updateContactData(qutim_sdk_0_3::Contact *contact) = 0;
	virtual void processEvent(ChangeEvent *ev) = 0;
protected:
	QScopedPointer<AbstractContactModelPrivate> d_ptr;
};

} // namespace SimpleContactList
} // namespace Core

Q_DECLARE_INTERFACE(Core::SimpleContactList::AbstractContactModel, "org.qutim.core.simplecontactlist.AbstractContactModel")

#endif // ABSTRACTCONTACTMODEL_H

