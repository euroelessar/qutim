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

#include "abstractcontactmodel_p.h"
#include <qutim/metacontactmanager.h>
#include <qutim/protocol.h>
#include <qutim/contact.h>
#include <qutim/event.h>
#include <qutim/chatsession.h>
#include <qutim/metacontact.h>
#include <qutim/mimeobjectdata.h>
#include <qutim/utils.h>
#include <QApplication>
#include <QTimer>
#include <QMimeData>

Q_DECLARE_METATYPE(QWeakPointer<qutim_sdk_0_3::Contact>)
#define CONTACT_PROPERTY "__contactList_contact"

namespace Core {
namespace SimpleContactList {

using namespace qutim_sdk_0_3;

typedef QWeakPointer<qutim_sdk_0_3::Contact> ContactPtr;

Contact *getRealUnit(Notification *notification)
{
	ContactPtr pointer = notification->property(CONTACT_PROPERTY).value<ContactPtr>();
	if (!pointer.isNull())
		return pointer.data();
	
	ChatUnit *unit = qobject_cast<ChatUnit*>(notification->request().object());
	if (!unit)
		return 0;

	static quint16 realUnitRequestEvent = Event::registerType("real-chatunit-request");
	Event event(realUnitRequestEvent);
	QCoreApplication::sendEvent(unit, &event);

	Contact *contact = event.at<Contact*>(0);
	while (unit && !contact) {
		if (!!(contact = qobject_cast<Contact*>(unit)))
			break;
		unit = unit->upperUnit();
	}

	if (Contact *meta = qobject_cast<MetaContact*>(contact ? contact->metaContact() : 0))
		contact = meta;

	pointer = ContactPtr(contact);
	notification->setProperty(CONTACT_PROPERTY, qVariantFromValue<ContactPtr>(pointer));

	return contact;
}

void NotificationsQueue::append(Notification *notification)
{
	Notification::Type type = notification->request().type();
	if (type == Notification::IncomingMessage ||
		type == Notification::OutgoingMessage ||
		type == Notification::ChatIncomingMessage ||
		type == Notification::ChatOutgoingMessage)
	{
		m_messageNotifications << notification;
	} else if (type == Notification::UserTyping) {
		m_typingNotifications << notification;
	} else {
		m_notifications << notification;
	}
}

bool NotificationsQueue::remove(Notification *notification)
{
	if (!m_messageNotifications.removeOne(notification)) {
		if (!m_notifications.removeOne(notification))
			return m_typingNotifications.removeOne(notification);
		else
			return true;
	} else {
		return true;
	}
}

Notification *NotificationsQueue::first() const
{
	// Message notifications have highest priority
	if (!m_messageNotifications.isEmpty())
		return m_messageNotifications.first();

	else if (!m_notifications.isEmpty())
		return m_notifications.first();

	// Typing notifications have lowest priority
	else if (!m_typingNotifications.isEmpty())
		return m_typingNotifications.first();

	return 0;
}

bool NotificationsQueue::isEmpty()
{
	return m_messageNotifications.isEmpty() &&
			m_notifications.isEmpty() &&
			m_typingNotifications.isEmpty();
}

QList<QList<Notification*> > NotificationsQueue::all()
{
	QList<QList<Notification*> > all;
	all << m_messageNotifications << m_notifications << m_typingNotifications;
	return all;
}

AbstractContactModel::AbstractContactModel(AbstractContactModelPrivate *d, QObject *parent) :
	QAbstractItemModel(parent), NotificationBackend("ContactList"), d_ptr(d)
{
	setDescription(QT_TR_NOOP("Blink icon in the contact list"));
	allowRejectedNotifications("confMessageWithoutUserNick");

	d->showNotificationIcon = false;
	Event::eventManager()->installEventFilter(this);

	ConfigGroup group = Config().group(QLatin1String("contactList"));
	d->showOffline = group.value(QLatin1String("showOffline"), true);
	QTimer::singleShot(0, this, SLOT(init()));

	d->mailIcon                = Icon(QLatin1String("mail-message-new-qutim"));
	d->typingIcon              = Icon(QLatin1String("im-status-message-edit"));
	d->chatUserJoinedIcon      = Icon(QLatin1String("list-add-user-conference"));
	d->chatUserLeftIcon        = Icon(QLatin1String("list-remove-user-conference"));
	d->qutimIcon               = Icon(QLatin1String("qutim"));
	d->transferCompletedIcon   = Icon(QLatin1String("document-save-filetransfer-comleted"));
	d->birthdayIcon            = Icon(QLatin1String("view-calendar-birthday"));
	d->defaultNotificationIcon = Icon(QLatin1String("dialog-information"));
}

AbstractContactModel::~AbstractContactModel()
{
}

bool AbstractContactModel::showOffline() const
{
	return d_func()->showOffline;
}

void AbstractContactModel::hideShowOffline()
{
	Q_D(AbstractContactModel);
	ConfigGroup group = Config().group("contactList");
	bool show = !group.value("showOffline", true);
	group.setValue("showOffline", show);
	group.sync();
	if (d->showOffline == show)
		return;
	d->showOffline = show;
	filterAllList();
}

QSet<QString> AbstractContactModel::selectedTags() const
{
	return d_func()->selectedTags;
}

QStringList AbstractContactModel::tags() const
{
	return QStringList();
}

void AbstractContactModel::filterList(const QString &filter)
{
	Q_D(AbstractContactModel);
	if (filter == d->lastFilter)
		return;
	d->lastFilter = filter;
	filterAllList();
}

void AbstractContactModel::filterList(const QStringList &tags_helper)
{
	Q_D(AbstractContactModel);
	QSet<QString> tags = QSet<QString>::fromList(tags_helper);
	if (tags == d->selectedTags)
		return;
	d->selectedTags = tags;
	filterAllList();
}

QVariant AbstractContactModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_D(const AbstractContactModel);
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section==0) {
		if (d->selectedTags.isEmpty())
			return tr("All tags");
		else
			return tr("Custom tags");
	}

	return QVariant();
}

int AbstractContactModel::columnCount(const QModelIndex &) const
{
	return 1;
}

Qt::ItemFlags AbstractContactModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = QAbstractItemModel::flags(index);

	ContactItemType type = getItemType(index);
	flags |= Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;
	if (type == ContactType)
		flags |= Qt::ItemIsEditable;

	return flags;
}

Qt::DropActions AbstractContactModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

bool AbstractContactModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
										int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(row);
	Q_UNUSED(column);
	Q_D(AbstractContactModel);
	if (action == Qt::IgnoreAction)
		return true;

	ContactItemType parentType = getItemType(parent);
	if (parentType != ContactType && parentType != TagType)
		return false;

	QString mimetype;
	bool isContact = data->hasFormat(QUTIM_MIME_CONTACT_INTERNAL);
	if (isContact)
		mimetype = QUTIM_MIME_CONTACT_INTERNAL;
	else if (data->hasFormat(QUTIM_MIME_TAG_INTERNAL))
		mimetype = QUTIM_MIME_TAG_INTERNAL;
	else
		return false;

	ItemHelper *item = decodeMimeData(data, mimetype);
	if (isContact && item->type != ContactType)
		return false;

	ChangeEvent *ev = new ChangeEvent;
	ev->child = item;
	ev->parent = reinterpret_cast<ItemHelper*>(parent.internalPointer());
	if (item->type == TagType) {
		ev->type = ChangeEvent::MoveTag;
	} else if (ev->parent->type == TagType) {
		ev->type = ChangeEvent::ChangeTags;
	} else if(item->type == ContactType && ev->parent->type == ContactType) {
		MetaContactManager * const manager = MetaContactManager::instance();
		if (!manager) {
			delete ev;
			return false;
		}
		ev->type = ChangeEvent::MergeContacts;
	} else {
		Q_ASSERT(!"Something is wrong with Drag&Drop");
		delete ev;
		return false;
	}
	d->events << ev;
	d->timer.start(1, this);

	return true;
}

void AbstractContactModel::timerEvent(QTimerEvent *timerEvent)
{
	Q_D(AbstractContactModel);
	if (timerEvent->timerId() == d->timer.timerId()) {
		for (int i = 0; i < d->events.size(); i++) {
			processEvent(d->events.at(i));
			delete d->events.at(i);
		}
		d->events.clear();
		d->timer.stop();
		return;
	} else if (timerEvent->timerId() == d->notificationTimer.timerId()) {
		foreach (Contact *contact, d->notifications.keys())
			updateContactData(contact);
		d->showNotificationIcon = !d->showNotificationIcon;
		return;
	}
	AbstractContactModel::timerEvent(timerEvent);
}

void AbstractContactModel::handleNotification(Notification *notification)
{
	Q_D(AbstractContactModel);
	Contact *contact = getRealUnit(notification);
	if (!contact)
		return;
	Q_ASSERT(getRealUnit(notification));

	if (d->notifications.isEmpty())
		d->notificationTimer.start(500, this);

	NotificationsQueue &notifications = d->notifications[contact];
	if (notifications.isEmpty())
		connect(contact, SIGNAL(destroyed()), SLOT(onContactDestroyed()));
	Notification *old = notifications.first();
	notifications.append(notification);
	ref(notification);
	connect(notification, SIGNAL(finished(qutim_sdk_0_3::Notification::State)),
			SLOT(onNotificationFinished()));

	if (notifications.first() != old)
		updateContactData(contact);
}

QIcon AbstractContactModel::getIconForNotification(Notification *notification) const
{
	Q_D(const AbstractContactModel);
	NotificationRequest request = notification->request();
	switch (request.type()) {
	case Notification::IncomingMessage:
	case Notification::OutgoingMessage:
	case Notification::ChatIncomingMessage:
	case Notification::ChatOutgoingMessage:
		return d->mailIcon;
	case Notification::UserTyping:
		return d->typingIcon;
	case Notification::UserOnline:
	case Notification::UserOffline:
	case Notification::UserChangedStatus:
		return request.property("previousStatus", Status(Status::Offline)).icon();
	case Notification::ChatUserJoined:
		return d->chatUserJoinedIcon;
	case Notification::ChatUserLeft:
		return d->chatUserLeftIcon;
	case Notification::AppStartup:
		return d->qutimIcon;
	case Notification::FileTransferCompleted:
		return d->transferCompletedIcon;
	case Notification::UserHasBirthday:
		return d->birthdayIcon;
	case Notification::BlockedMessage:
	case Notification::System:
	case Notification::Attention:
		return d->defaultNotificationIcon;
	}
	return QIcon();
}

void AbstractContactModel::setEncodedData(QMimeData *mimeData, const QString &type, const QModelIndex &index)
{
	QByteArray encodedData;
	encodedData.resize(sizeof(ptrdiff_t));
	void *internalId = index.internalPointer();
	qMemCopy(encodedData.data(), &internalId, sizeof(void*));
	mimeData->setData(type, encodedData);
}

ItemHelper *AbstractContactModel::decodeMimeData(const QMimeData *mimeData, const QString &type)
{
	QByteArray encodedData = mimeData->data(type);
	Q_ASSERT(encodedData.size() == sizeof(void*));
	void *internalId = *reinterpret_cast<void**>(encodedData.data());
	ItemHelper *item = reinterpret_cast<ItemHelper*>(internalId);
	return item;
}

void AbstractContactModel::init()
{
	MetaContactManager * const manager = MetaContactManager::instance();
	if (manager) {
		connect(MetaContactManager::instance(), SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
				this, SLOT(addContact(qutim_sdk_0_3::Contact*)));
	}
}

void AbstractContactModel::onNotificationFinished()
{
	Q_D(AbstractContactModel);
	Notification *notification = sender_cast<Notification*>(sender());
	Contact *contact = getRealUnit(notification);
	Q_ASSERT(contact);
	deref(notification);

	QHash<Contact*, NotificationsQueue>::iterator it = d->notifications.find(contact);
	if (it == d->notifications.end())
		return;

	Notification *old = it->first();
	it->remove(notification);
	if (old == notification)
		updateContactData(it.key());
	if (it->isEmpty()) {
		d->notifications.erase(it);
		disconnect(contact, SIGNAL(destroyed()), this, SLOT(onContactDestroyed()));
	}
	if (d->notifications.isEmpty())
		d->notificationTimer.stop();
}

void AbstractContactModel::onContactDestroyed()
{
	Q_D(AbstractContactModel);
	Contact *contact = static_cast<Contact*>(sender());
	QHash<Contact*, NotificationsQueue>::iterator it = d->notifications.find(contact);
	if (it != d->notifications.end()) {
		const QList<QList<Notification*> > all = it->all();
		d->notifications.erase(it);
        foreach (const QList<Notification*> &notifications, all) {
			foreach (Notification *notification, notifications) {
				disconnect(notification, 0, this, 0);
                deref(notification);
			}
		}
	}
	if (d->notifications.isEmpty())
		d->notificationTimer.stop();
}

} // namespace SimpleContactList
} // namespace Core

