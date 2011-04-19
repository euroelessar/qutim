/****************************************************************************
 *  abstractcontactmodel.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "abstractcontactmodel_p.h"
#include <qutim/metacontactmanager.h>
#include <qutim/protocol.h>
#include <qutim/contact.h>
#include <qutim/event.h>
#include <qutim/chatsession.h>
#include <qutim/metacontact.h>
#include <qutim/mimeobjectdata.h>
#include <QApplication>
#include <QTimer>
#include <QMimeData>

namespace Core {
namespace SimpleContactList {

using namespace qutim_sdk_0_3;

AbstractContactModel::AbstractContactModel(AbstractContactModelPrivate *d, QObject *parent) :
	QAbstractItemModel(parent), d_ptr(d)
{
	d->showMessageIcon = false;
	Event::eventManager()->installEventFilter(this);
	d->realUnitRequestEvent = Event::registerType("real-chatunit-request");
	d->unreadIcon = Icon(QLatin1String("mail-unread-new"));
	ConfigGroup group = Config().group("contactList");
	d->showOffline = group.value("showOffline", true);
	QTimer::singleShot(0, this, SLOT(init()));
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
	if (item->type == TagType)
		ev->type = ChangeEvent::MoveTag;
	else if (item->type == TagType)
		ev->type = ChangeEvent::ChangeTags;
	else if(item->type == ContactType)
		ev->type = ChangeEvent::MergeContacts;
	d->events << ev;
	d->timer.start(1, this);

	return true;
	// We should return false
	//			return false;
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
	} else if (timerEvent->timerId() == d->unreadTimer.timerId()) {
		foreach (Contact *contact, d->unreadContacts)
			updateContactData(contact);
		d->showMessageIcon = !d->showMessageIcon;
		return;
	}
	AbstractContactModel::timerEvent(timerEvent);
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

void AbstractContactModel::onUnreadChanged(const qutim_sdk_0_3::MessageList &messages)
{
	Q_D(AbstractContactModel);
	ChatSession *session = qobject_cast<ChatSession*>(sender());

	QSet<Contact*> contacts;
	QSet<ChatUnit*> chatUnits;
	for (int i = 0; i < messages.size(); i++) {
		ChatUnit *unit = messages.at(i).chatUnit();
		if (chatUnits.contains(unit) || !unit)
			continue;
		chatUnits.insert(unit);
		Event event(d->realUnitRequestEvent);
		QCoreApplication::sendEvent(unit, &event);
		Contact *contact = event.at<Contact*>(0);
		while (unit && !contact) {
			if (!!(contact = qobject_cast<Contact*>(unit)))
				break;
			unit = unit->upperUnit();
		}
		if (Contact *meta = qobject_cast<MetaContact*>(contact->metaContact()))
			contact = meta;
		if (contact)
			contacts.insert(contact);
	}
	if (contacts.isEmpty())
		d->unreadBySession.remove(session);
	else
		d->unreadBySession.insert(session, contacts);
	foreach (const QSet<Contact*> &contactsSet, d->unreadBySession)
		contacts |= contactsSet;

	if (!contacts.isEmpty() && !d->unreadTimer.isActive())
		d->unreadTimer.start(500, this);
	else if (contacts.isEmpty())
		d->unreadTimer.stop();

	if (!d->showMessageIcon) {
		d->unreadContacts = contacts;
	} else {
		QSet<Contact*> needUpdate = d->unreadContacts;
		needUpdate.subtract(contacts);
		d->unreadContacts = contacts;
		foreach (Contact *contact, needUpdate)
			updateContactData(contact);
	}
}

void AbstractContactModel::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),
			this, SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
}

void AbstractContactModel::init()
{
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	connect(MetaContactManager::instance(), SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			this, SLOT(addContact(qutim_sdk_0_3::Contact*)));
}

} // namespace SimpleContactList
} // namespace Core
