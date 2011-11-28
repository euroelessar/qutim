/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "jcontactresource_p.h"
#include "jcontact.h"
#include "../jaccount.h"
#include "../../jprotocol.h"
#include "jmessagesession.h"
#include "jmessagehandler.h"
#include <qutim/status.h>
#include <qutim/inforequest.h>
#include <qutim/tooltip.h>
#include <QStringBuilder>
#include <jreen/chatstate.h>
#include <jreen/message.h>
#include <jreen/client.h>

using namespace qutim_sdk_0_3;

namespace Jabber
{

JContactResource::JContactResource(ChatUnit *parent, const QString &name) :
	Buddy(parent->account()), d_ptr(new JContactResourcePrivate(parent))
{
	Q_D(JContactResource);
	d->name = name;
	d->id = parent->id() % QLatin1Char('/') % name;
	connect(parent, SIGNAL(avatarChanged(QString)), this, SIGNAL(avatarChanged(QString)));
}

JContactResource::JContactResource(ChatUnit *parent, JContactResourcePrivate &ptr) :
	Buddy(parent->account()), d_ptr(&ptr)
{
}

JContactResource::~JContactResource()
{
}

QString JContactResource::name() const
{
	return d_func()->name;
}

QString JContactResource::id() const
{
	return d_func()->id;
}

QString JContactResource::title() const
{
	Q_D(const JContactResource);
	if (Contact *contact = qobject_cast<Contact *>(d->contact)) {
		return contact->title() % "/" % d->name;
	} else {
		return Buddy::title();
	}
}

bool JContactResource::sendMessage(const qutim_sdk_0_3::Message &message)
{
	JAccount *a = static_cast<JAccount*>(account());

	if(a->status() == Status::Offline || a->status() == Status::Connecting)
		return false;
	qDebug("%s", Q_FUNC_INFO);

	a->messageSessionManager()->sendMessage(this, message);
	return true;
}

void JContactResource::setPriority(int priority)
{
	d_func()->presence.setPriority(priority);
}

int JContactResource::priority()
{
	return d_func()->presence.priority();
}

void JContactResource::setStatus(const Jreen::Presence presence)
{
	Q_D(JContactResource);
	Status current = status();
	d->presence = presence;
	emit statusChanged(status(), current);
}

Status JContactResource::status() const
{
	Q_D(const JContactResource);
	Status status = JStatus::presenceToStatus(d->presence.subtype());
	status.setText(d->presence.status());
	status.setExtendedInfos(d->extInfo);
	return status;
}

bool JContactResource::event(QEvent *ev)
{
	if (ev->type() == ChatStateEvent::eventType()) {
		ChatStateEvent *chatEvent = static_cast<ChatStateEvent *>(ev);
		Jreen::ChatState::State state = static_cast<Jreen::ChatState::State>(chatEvent->chatState());

		Jreen::Message msg(Jreen::Message::Chat,
						   d_func()->id);
		msg.addExtension(new Jreen::ChatState(state));
		JAccount *account = static_cast<JAccount*>(d_func()->contact->account());
		account->messageSessionManager()->send(msg);
		return true;
	} else if (ev->type() == ToolTipEvent::eventType()) {
		ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
		event->addField(QT_TRANSLATE_NOOP("ContactResource", "Resource"),
						QString("%1 (%2)").arg(id()).arg(priority()), 75);
		if (!text().isEmpty())
			event->addField(text(), QString());
		event->addHtml("<font size=-1>", 50);
		QString client = property("client").toString();
		if (!client.isEmpty()) {
			event->addField(QT_TRANSLATE_NOOP("ContactResource", "Possible client"),
							client,
							property("clientIcon").toString(),
							ToolTipEvent::IconBeforeDescription,
							25);
			QString os = property("os").toString();
			if (!os.isEmpty())
				event->addField(QT_TRANSLATE_NOOP("ContactResource", "OS"), os, 25);
		}
		event->addHtml("</font>", 10);
		return true;
	}
	return Buddy::event(ev);
}

QSet<QString> JContactResource::features() const
{
	return d_ptr->features;
}

void JContactResource::setFeatures(const QSet<QString> &features)
{
	d_ptr->features = features;
}

bool JContactResource::checkFeature(const QLatin1String &feature) const
{
	return d_ptr->features.contains(feature);
}

bool JContactResource::checkFeature(const QString &feature) const
{
	return d_ptr->features.contains(feature);
}

bool JContactResource::checkFeature(const std::string &feature) const
{
	return d_ptr->features.contains(QString::fromStdString(feature));
}

ChatUnit *JContactResource::upperUnit()
{
	return qobject_cast<Contact *>(d_ptr->contact);
}

QString JContactResource::avatar() const
{
	if (Buddy *buddy = qobject_cast<Buddy*>(d_func()->contact))
		return buddy->avatar();
	return QString();
}

QString JContactResource::text() const
{
	return d_func()->presence.status();
}

void JContactResource::setExtendedInfo(const QString &name, const QVariantHash &info)
{
	Q_D(JContactResource);
	Status current = status();
	d->extInfo.insert(name, info);
	emit statusChanged(status(), current);
}

void JContactResource::removeExtendedInfo(const QString &name)
{
	Q_D(JContactResource);
	Status current = status();
	d->extInfo.remove(name);
	emit statusChanged(status(), current);
}

}

