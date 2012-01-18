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

#include "vcontact.h"
#include "vconnection.h"
#include "vaccount.h"
#include "vmessages.h"
#include "vroster.h"
#include "vinforequest.h"
#include <qutim/tooltip.h>
#include <qutim/inforequest.h>
#include <qutim/notification.h>

class VContactPrivate
{
public:
	bool online;
	QString id;
	bool inList;
	QStringList tags;
	QList<int> tagIds;
	QString name;
	QString avatar;
	QString activity;
	VAccount *account;
};


VContact::VContact(const QString& id, VAccount* account): Contact(account), d_ptr(new VContactPrivate)
{
	Q_D(VContact);
	d->id = id;
	d->account = account;
	d->online = false;
	d->inList = false;
}


QString VContact::id() const
{
	return d_func()->id;
}

bool VContact::isInList() const
{
	return d_func()->inList;
}

bool VContact::sendMessage(const Message& message)
{
	Q_D(VContact);
	if (d->account->connection()->connectionState() != Connected)
		return false;
	d_func()->account->connection()->messages()->sendMessage(message);
	return true;
}

void VContact::setTags(const QStringList& tags)
{
	Q_UNUSED(tags);
}

void VContact::setInList(bool inList)
{
	Q_UNUSED(inList);
}

void VContact::setContactTags(const QStringList& tags)
{
	Q_D(VContact);
	if (d->tags != tags) {
		QStringList previous = d->tags;
		d->tags = tags;
		emit tagsChanged(tags, previous);
	}
}

void VContact::setContactInList(bool inList)
{
	Q_D(VContact);
	if (d->inList != inList) {
		d->inList = inList;
		emit inListChanged(d->inList);
	}
}

Status VContact::status() const
{
	Q_D(const VContact);
	Status status = Status::instance(d->online ? Status::Online : Status::Offline, "vkontakte");
	status.setText(d->activity);
	return status;
}

void VContact::setOnline(bool set)
{
	Q_D(VContact);
	if (d->online != set) {
		Status previous = status();
		d->online = set;
		Status status = this->status();
		setChatState(set ? ChatStateInActive : ChatStateGone);
		NotificationRequest request(this, status, previous);
		request.send();
		emit statusChanged(status, previous);
	}
}

void VContact::setActivity(const QString &activity)
{
	Q_D(VContact);
	if (d->activity != activity) {
		Status previous = status();
		d->activity = activity;
		emit statusChanged(status(), previous);
	}
}

QString VContact::activity() const
{
	return d_func()->activity;
}

VContact::~VContact()
{

}

QStringList VContact::tags() const
{
	return d_func()->tags;
}

QString VContact::name() const
{
	return d_func()->name;
}

void VContact::setContactName(const QString& name)
{
	Q_D(VContact);
	if (d->name != name) {
		QString previous = d->name;
		d->name = name;
		emit nameChanged(name, previous);
	}
}

void VContact::setName(const QString& name)
{
	Q_UNUSED(name);
}

void VContact::setAvatar(const QString &avatar)
{
	Q_D(VContact);
	if (d->avatar != avatar) {
		d->avatar = avatar;
		emit avatarChanged(avatar);
	}
}

QString VContact::avatar() const
{
	return d_func()->avatar;
}

bool VContact::event(QEvent *ev)
{
	Q_D(VContact);
	if (ev->type() == ToolTipEvent::eventType()) {
		ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
		QString mobile = property("mobilePhone").toString();
		if (!mobile.isEmpty())
			event->addField(QT_TRANSLATE_NOOP("ContactInfo", "Mobile phone"),
							mobile,
							ExtensionIcon("phone"));
		if (!d->activity.isEmpty())
			event->addField(QT_TRANSLATE_NOOP("ContactInfo","Activity"),
							d->activity);
	}
	return Contact::event(ev);
}

VAccount *VContact::account() const
{
	return d_func()->account;
}

