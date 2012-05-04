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
#include "jcontactresource_p.h"
#include "jcontact.h"
#include "../jaccount.h"
#include "../../jprotocol.h"
#include "jmessagesession.h"
#include "jmessagehandler.h"
#include <qutim/status.h>
#include <qutim/inforequest.h>
#include <qutim/tooltip.h>
#include <qutim/debug.h>
#include <QStringBuilder>
#include <jreen/chatstate.h>
#include <jreen/message.h>
#include <jreen/client.h>
#ifdef JABBER_HAVE_MULTIMEDIA
# include <jreen/experimental/jinglemanager.h>
#endif

using namespace qutim_sdk_0_3;

namespace Jabber
{

JContactResource::JContactResource(ChatUnit *parent, const QString &name) :
	Buddy(parent->account()), d_ptr(new JContactResourcePrivate(parent))
{
	Q_D(JContactResource);
	d->name = name;
	d->id = parent->id() % QLatin1Char('/') % name;
	d->pgpVerifyStatus = QCA::SecureMessageSignature::NoKey;
	connect(parent, SIGNAL(avatarChanged(QString)), this, SIGNAL(avatarChanged(QString)));
}

JContactResource::JContactResource(ChatUnit *parent, JContactResourcePrivate &ptr) :
	Buddy(parent->account()), d_ptr(&ptr)
{
}

JContactResource::JContactResource(JAccount *account, const QString &name) :
    Buddy(account), d_ptr(new JContactResourcePrivate(0))
{
	Q_D(JContactResource);
	d->name = name;
	d->id = account->id() % QLatin1Char('/') % name;
	d->pgpVerifyStatus = QCA::SecureMessageSignature::NoKey;
	connect(account, SIGNAL(avatarChanged(QString)), this, SIGNAL(avatarChanged(QString)));
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
		return contact->title() % QLatin1Char('/') % d->name;
	} else if (Account *account = qobject_cast<Account*>(d->contact)) {
		return account->name() % QLatin1Char('/') % d->name;
	} else {
		return Buddy::title();
	}
}

bool JContactResource::sendMessage(const qutim_sdk_0_3::Message &message)
{
	JAccount *a = static_cast<JAccount*>(account());

	if(a->status() == Status::Offline || a->status() == Status::Connecting)
		return false;
	debug() << Q_FUNC_INFO;

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

QCA::PGPKey JContactResource::pgpKey() const
{
	return d_func()->pgpKey;
}

void JContactResource::setPGPKey(const QCA::PGPKey &key)
{
	d_func()->pgpKey = key;
}

QCA::SecureMessageSignature::IdentityResult JContactResource::pgpVerifyStatus() const
{
	return d_func()->pgpVerifyStatus;
}

void JContactResource::setPGPVerifyStatus(QCA::SecureMessageSignature::IdentityResult pgpVerifyStatus)
{
	d_func()->pgpVerifyStatus = pgpVerifyStatus;
}

Jreen::Presence::Type JContactResource::presenceType() const
{
	return d_func()->presence.subtype();
}

Jreen::Presence JContactResource::presence() const
{
	return d_func()->presence;
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
		JAccount *account = qobject_cast<JAccount*>(d_func()->contact);
		if (!account)
			account = static_cast<JAccount*>(static_cast<ChatUnit*>(d_func()->contact)->account());
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

bool JContactResource::canCall() const
{
#ifdef JABBER_HAVE_MULTIMEDIA
	Jreen::Client *client = static_cast<const JAccount*>(account())->client();
	return client->jingleManager()->checkSupport(d_ptr->features);
#else
	return false;
#endif
}

void JContactResource::call(const QStringList &contents)
{
	Q_UNUSED(contents);
#ifdef JABBER_HAVE_MULTIMEDIA
	Jreen::Client *client = static_cast<JAccount*>(account())->client();
	client->jingleManager()->createSession(d_func()->id, contents);
#endif
}

ChatUnit *JContactResource::upperUnit()
{
	return qobject_cast<Contact *>(d_ptr->contact);
}

QString JContactResource::avatar() const
{
	if (Buddy *buddy = qobject_cast<Buddy*>(d_func()->contact))
		return buddy->avatar();
	if (JAccount *account = qobject_cast<JAccount*>(d_func()->contact))
		return account->avatar();
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

