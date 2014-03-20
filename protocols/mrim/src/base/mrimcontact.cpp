/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Rusanov Peter <peter.rusanov@gmail.com>
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

#include <QSet>

#include "mrimaccount.h"
#include "roster.h"
#include "mrimcontact.h"
#include <QBasicTimer>
#include <qutim/authorizationdialog.h>
#include <qutim/notification.h>

struct MrimContactPrivate
{
	MrimContactPrivate()
		: id(-1), groupId(-1), serverFlags(0), inList(true), contactFlags(0), featureFlags(0) {}
    qint32 id;
    QString name;
    QString email;
	QBasicTimer composingTimer;
	QBasicTimer incomingComposingTimer;
    qint32 groupId;
    quint32 serverFlags;
	bool inList;
    MrimContact::ContactFlags contactFlags;
    MrimConnection::FeatureFlags featureFlags;
    MrimUserAgent userAgent;
    MrimStatus status;
};

MrimContact::MrimContact(const QString &email, MrimAccount *account)
    : Contact(account), p(new MrimContactPrivate)
{
	p->email = email;
}

MrimContact::~MrimContact()
{ }

//from Contact
QString MrimContact::id() const
{
	return isPhone() ? email() + QString::number(p->id) : email();
}

QString MrimContact::name() const
{
	return p->name;
}

void MrimContact::setName(const QString &name)
{
	Q_UNUSED(name);
}

void MrimContact::setContactName(const QString &name)
{
	if (name == p->name)
		return;
	QString oldName = p->name;
	p->name = name;
	emit nameChanged(name, oldName);
}

QStringList MrimContact::tags() const
{
	QStringList groups;
	if (p->inList)
		groups << account()->roster()->groupName(p->groupId);
	return groups;
}

void MrimContact::setTags(const QStringList &tags)
{
    Q_UNUSED(tags);
    //do nothing
}

bool MrimContact::sendMessage(const Message &message)
{
	if (account()->status() == Status::Offline)
		return false;
    account()->connection()->messages()->send(this, message);
	return true;
}

bool MrimContact::isInList() const
{
	return p->inList;
}

void MrimContact::setInList(bool inList)
{
	Q_UNUSED(inList);
} //not supported yet

void MrimContact::setContactInList(bool inList)
{
	if (p->inList == inList)
		return;
	QStringList groups = tags();
	p->inList = inList;
	emit inListChanged(inList);
	emit tagsChanged(tags(), groups);
}

//new
const MrimAccount *MrimContact::account() const
{
	return static_cast<const MrimAccount*>(Contact::account());
}

MrimAccount *MrimContact::account()
{
	return static_cast<MrimAccount*>(Contact::account());
}

quint32 MrimContact::contactId() const
{
	return p->id;
}

void MrimContact::setContactId(quint32 id)
{
	p->id = id;
}

quint32 MrimContact::groupId() const
{
	return p->groupId;
}

void MrimContact::setGroupId(quint32 id)
{
    if (id > MRIM_MAX_GROUPS && id != MRIM_PHONE_GROUP_ID) {
        id = MRIM_DEFAULT_GROUP_ID;
    }
	QStringList oldTags = tags();
	p->groupId = id;
	emit tagsChanged(tags(), oldTags);
}

MrimContact::ContactFlags MrimContact::flags() const
{ return p->contactFlags; }

void MrimContact::setFlags(ContactFlags flags)
{ p->contactFlags = flags; }

quint32 MrimContact::serverFlags() const
{ return p->serverFlags; }

void MrimContact::setServerFlags(quint32 flags)
{ p->serverFlags = flags; }

QString MrimContact::email() const
{ return p->email; }

void MrimContact::setEmail(const QString& email)
{ p->email = email; }

MrimConnection::FeatureFlags MrimContact::featureFlags() const
{ return p->featureFlags; }

void MrimContact::setFeatureFlags(MrimConnection::FeatureFlags flags)
{ p->featureFlags = flags; }

const MrimUserAgent& MrimContact::userAgent() const
{ return p->userAgent; }

void MrimContact::setUserAgent(const MrimUserAgent& agent)
{
	p->userAgent = agent;
	emit userAgentChanged(p->userAgent);
}

bool MrimContact::isPhone() const
{ return p->groupId == MRIM_PHONE_GROUP_ID; }

Status MrimContact::status() const
{
	return p->status;
}

MrimStatus MrimContact::mrimStatus() const
{
	return p->status;
}

void MrimContact::setStatus(const MrimStatus& status)
{
	MrimStatus previous = p->status;
    p->status = status;
	NotificationRequest request(this, status, previous);
	request.send();
	emit statusChanged(p->status, previous);
}

void MrimContact::clearComposingState()
{
	bool isComposing = chatState() == ChatUnit::ChatStateComposing;
	p->incomingComposingTimer.stop();
	if (isComposing)
		setChatState(ChatUnit::ChatStateActive);
}

void MrimContact::updateComposingState()
{
	bool isComposing = chatState() == ChatUnit::ChatStateComposing;
	p->incomingComposingTimer.start(10000, this);
	if (!isComposing)
		setChatState(ChatUnit::ChatStateComposing);
}

bool MrimContact::event(QEvent *ev)
{
	if (ev->type() == ChatStateEvent::eventType()) {
		ChatStateEvent *chatEvent = static_cast<ChatStateEvent *>(ev);
		bool isComposing = chatEvent->chatState() == ChatUnit::ChatStateComposing;
		if (p->composingTimer.isActive() == isComposing)
			return true;
		if (isComposing) {
			// We should send composing notification every 10 secs
			p->composingTimer.start(10000, this);
			account()->connection()->messages()->sendComposingNotification(this);
		} else {
			p->composingTimer.stop();
		}
		return true;
	} else if(ev->type() == Authorization::Reply::eventType()) {
		Authorization::Reply *reply = static_cast<Authorization::Reply*>(ev);
		if(reply->replyType() != Authorization::Reply::Accept)
			return true;
		MrimPacket packet(MrimPacket::Compose);
		packet.setMsgType(MRIM_CS_AUTHORIZE);
		packet.append(p->email);
		account()->connection()->sendPacket(packet);
		return true;
	}
	return Contact::event(ev);
}

void MrimContact::timerEvent(QTimerEvent *event)
{
	if (p->composingTimer.timerId() == event->timerId()) {
		account()->connection()->messages()->sendComposingNotification(this);
		return;
	} else if (p->incomingComposingTimer.timerId() == event->timerId()) {
		setChatState(ChatUnit::ChatStateActive);
		p->incomingComposingTimer.stop();
		return;
	}
	return Contact::timerEvent(event);
}

QDebug operator<<(QDebug dbg, const MrimContact &c)
{
	dbg.nospace() << "MrimContact (email=" << c.email() << ", name=" << c.name()
				  << ", groupId=" << c.groupId()
				  << ", contactId=" << c.contactId()
				  << ", flags=0x" << hex << c.flags()
				  << ", serverFlags=0x" << hex << c.serverFlags()
				  << ", featureFlags=0x" << hex << c.featureFlags()
				  << ", userAgent=" << c.userAgent().toReadable()
				  << ", account=" << c.account()->id()
				  << ")";

	return dbg.space();
}

