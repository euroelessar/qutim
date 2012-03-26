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

#include "icqcontact_p.h"
#include "messages.h"
#include "buddycaps.h"
#include "icqaccount_p.h"
#include "metainfo/infometarequest.h"
#include "qutim/chatsession.h"
#include "qutim/notification.h"
#include "qutim/tooltip.h"
#include "qutim/extensionicon.h"
#include <QApplication>
#include <QVariant>
#include <qutim/authorizationdialog.h>

namespace qutim_sdk_0_3 {

namespace oscar {

ChatStateUpdater::ChatStateUpdater()
{
	m_timer.setInterval(500);
	connect(&m_timer, SIGNAL(timeout()), SLOT(sendState()));
}

void ChatStateUpdater::updateState(IcqContact *contact, ChatState state)
{
	m_states.insert(contact, state);
	if (!m_timer.isActive())
		m_timer.start();
}

void ChatStateUpdater::sendState()
{
	QMutableHashIterator<QWeakPointer<IcqContact>, ChatState> it(m_states);
	while (it.hasNext()) {
		IcqContact *contact = it.next().key().data();
		if (!contact) {
			it.remove(); // Contact is destroyed
			continue;
		}
		Status::Type status = contact->account()->status().type();
		if (status == Status::Offline || status == Status::Connecting) {
			it.remove(); // We can't send any packets in offline or connecting state.
			continue;
		}
		if (contact->account()->connection()->testRate(MessageFamily, MessageMtn)) {
			sendState(contact, it.value());
			it.remove();
		}
	}
	if (m_states.isEmpty())
		m_timer.stop();
}

void ChatStateUpdater::sendState(IcqContact *contact, ChatState state)
{
	MTN type = MtnUnknown;
	if (state == ChatStatePaused)
		type = MtnTyped;
	else if (state == ChatStateComposing)
		type = MtnBegun;
	else if (state == ChatStateGone)
		type = MtnGone;
	else if (state == ChatStateInActive || state == ChatStateActive)
		type = MtnFinished;
	if (type == MtnUnknown)
		return;
	SNAC sn(MessageFamily, MessageMtn);
	sn.append(Cookie(true));
	sn.append<quint16>(1); // channel?
	sn.append<quint8>(contact->id());
	sn.append<quint16>(type);
	contact->account()->connection()->send(sn);
}

Q_GLOBAL_STATIC(ChatStateUpdater, chatStateUpdater)

void IcqContactPrivate::clearCapabilities()
{
	flags = 0;
	capabilities.clear();
}

void IcqContactPrivate::requestNick()
{
	Q_Q(IcqContact);
	if (name.isEmpty()) {
		ShortInfoMetaRequest *req = new ShortInfoMetaRequest(q);
		QObject::connect(req, SIGNAL(done(bool)), q, SLOT(infoReceived(bool)));
		req->send();
	}
}

void IcqContactPrivate::setCapabilities(const Capabilities &caps)
{
	clearCapabilities();
	foreach(const Capability &capability, caps)
	{
		if (capability.match(ICQ_CAPABILITY_HTMLMSGS))
			flags |= html_support;
		else if (capability.match(ICQ_CAPABILITY_UTF8))
			flags |= utf8_support;
		else if (capability.match(ICQ_CAPABILITY_SRVxRELAY))
			flags |= srvrelay_support;
	}
	capabilities = caps;
	emit q_ptr->capabilitiesChanged(caps);
}

FeedbagItem IcqContactPrivate::getNotInListGroup()
{
	FeedbagItem group = account->feedbag()->groupItem(QLatin1String("Default Group"), Feedbag::GenerateId);
	if (!group.isInList()) {
		group.setField(SsiGroupDefault);
		group.add();
	}
	return group;
}

IcqContact::IcqContact(const QString &uin, IcqAccount *account) :
	Contact(account), d_ptr(new IcqContactPrivate)
{
	Q_D(IcqContact);
	d->q_ptr = this;
	d->account = account;
	d->uin = uin;
	d->isInList = false;
	d->status = Status::Offline;
	d->clearCapabilities();
	d->state = ChatStateGone;
}

IcqContact::~IcqContact()
{

}

QStringList IcqContact::tags() const
{
	Q_D(const IcqContact);
	return d->tags;
}

QString IcqContact::id() const
{
	Q_D(const IcqContact);
	return d->uin;
}

QString IcqContact::name() const
{
	Q_D(const IcqContact);
	return d->name;
}

Status IcqContact::status() const
{
	Q_D(const IcqContact);
	return d->status;
}

QString IcqContact::avatar() const
{
	return d_func()->avatar;
}

QString IcqContact::proto() const
{
	return d_func()->proto;
}

bool IcqContact::isInList() const
{
	return d_func()->isInList;
}

bool IcqContact::sendMessage(const Message &message)
{
	if (account()->status() == Status::Offline)
		return false;
	return account()->d_func()->messageSender->appendMessage(this, message);
}

void IcqContact::setName(const QString &name)
{
	Q_D(IcqContact);
	
	FeedbagItem item = d->account->feedbag()->buddyForChange(d->uin);
	if (!item.isInList())
		return;
	
	if (!name.isEmpty())
		item.setField(SsiBuddyNick, name);
	else
		item.removeField(SsiBuddyNick);
	item.update();
	
	
//	if (d->item.isNull())
//		return;
//	d->name = name;
//	d->requestNick(); // requestNick will do nothing if the name is not empty
//	if (!name.isEmpty())
//		d->item.setField(SsiBuddyNick, name);
//	else
//		d->item.removeField(SsiBuddyNick);
//	if (d->item.isInList())
//		d->item.update();
////	foreach (FeedbagItem item, d->items) {
////		if (!name.isEmpty())
////			item.setField(SsiBuddyNick, name);
////		else
////			item.removeField(SsiBuddyNick);
////		item.update();
//	}
}

void IcqContact::setTags(const QStringList &tags)
{
	Q_D(IcqContact);
	if (d->tags == tags)
		return;
	
	Feedbag *feedbag = d->account->feedbag();
	FeedbagItem item = feedbag->buddyForChange(d->uin);
	if (!item.isInList())
		return;
	
	FeedbagItem groupItem;
	foreach (const QString &tag, tags) {
		groupItem = feedbag->groupItem(tag);
		if (!groupItem.isNull())
			break;
	}
	if (groupItem.isNull()) {
		QString group = tags.value(0);
		FeedbagItem groupItem;
		if (group.isEmpty())
			groupItem = d->getNotInListGroup();
		else
			groupItem = feedbag->groupItem(group, Feedbag::GenerateId);
		if (!groupItem.isInList())
			groupItem.add();
	}
	
	if (item.groupId() != groupItem.groupId()) {
		FeedbagItem oldItem = item;
		oldItem.remove();
		
		item.setInList(false);
		item.setId(feedbag->uniqueItemId(SsiBuddy));
		item.setGroup(groupItem.groupId());
	}
	
	TLV tagsData(SsiBuddyTags);
	foreach (const QString &tag, tags)
		tagsData.append<quint16>(tag);
	item.setField(tagsData);
	item.updateOrAdd();
}

void IcqContact::setInList(bool inList)
{
	Q_D(IcqContact);
	if (inList == isInList())
		return;
	FeedbagItem item = d->account->feedbag()->buddyForChange(d->uin);
	if (inList) {
		if (item.isInList())
			return;
		FeedbagItem group = d->getNotInListGroup();
		item.setGroup(group.groupId());
		item.add();
	} else {
		if (!item.isInList())
			return;
		item.remove();
	}
}

IcqAccount *IcqContact::account()
{
	return d_func()->account;
}

const Capabilities &IcqContact::capabilities() const
{
	return d_func()->capabilities;
}

const DirectConnectionInfo &IcqContact::dcInfo() const
{
	return d_func()->dc_info;
}

void IcqContact::setAvatar(const QString &avatar)
{
	Q_D(IcqContact);
	d->avatar = avatar;
	emit avatarChanged(avatar);
}

void IcqContact::setStatus(const Status &status, bool notification)
{
	Q_D(IcqContact);
	Status previous = d->status;
	d->status = status;
	if (status == Status::Offline) {
		d->clearCapabilities();
		emit capabilitiesChanged(Capabilities());
		d->onlineSince = QDateTime();
		d->awaySince = QDateTime();
		d->regTime = QDateTime();
	}

	if (notification &&
		(status.subtype() != previous.subtype() ||
		 status.text() != previous.text()))
	{
		NotificationRequest request(this, status, previous);
		request.send();
	}
	emit statusChanged(status, previous);
}

ChatState IcqContact::chatState() const
{
	return d_func()->state;
}

void IcqContact::updateFromItem()
{
	Q_D(IcqContact);
	FeedbagItem item = d->account->feedbag()->item(SsiBuddy, d->uin);
}

bool IcqContact::event(QEvent *ev)
{
	Q_D(IcqContact);
	if (ev->type() == ChatStateEvent::eventType()) {
		ChatStateEvent *chatEvent = static_cast<ChatStateEvent *>(ev);
		chatStateUpdater()->updateState(this, chatEvent->chatState());
		return true;
	} else if (ev->type() == ToolTipEvent::eventType()) {
		ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
		QString statusText = status().text();
		if (!statusText.isEmpty())
			event->addField(QString(), statusText, 80);
		QDateTime time;
		if (!d->onlineSince.isNull()) {
			time = QDateTime::currentDateTime();
			time = time.addSecs(-static_cast<int>(d->onlineSince.toTime_t()));
			time = time.toUTC();
			event->addField(QT_TRANSLATE_NOOP("ContactList", "Online time"),
							QString("%1d %2h %3m %4s")
							.arg(time.date().day() - 1)
							.arg(time.time().hour())
							.arg(time.time().minute())
							.arg(time.time().second()),
							30);
			event->addField(QT_TRANSLATE_NOOP("ContactList", "Signed on"),
							d->onlineSince.toLocalTime().toString(Qt::DefaultLocaleShortDate),
							30);
		}
		if (!d->awaySince.isNull()) {
			event->addField(QT_TRANSLATE_NOOP("ContactList", "Away since"),
							d->awaySince.toLocalTime().toString(Qt::DefaultLocaleShortDate),
							30);
		}
		if (!d->regTime.isNull()) {
			event->addField(QT_TRANSLATE_NOOP("ContactList", "Reg. date"),
							d->regTime.toLocalTime().toString(Qt::DefaultLocaleShortDate),
							30);
		}
	} else if(ev->type() == Authorization::Reply::eventType()) {
		Authorization::Reply *reply = static_cast<Authorization::Reply*>(ev);
		debug() << "handle auth reply" << (reply->replyType() == Authorization::Reply::Accept);
		bool answer = (reply->replyType() == Authorization::Reply::Accept);
		SNAC snac(ListsFamily, ListsCliAuthResponse);
		snac.append<quint8>(id()); // uin.
		snac.append<quint8>(answer ? 0x01 : 0x00); // auth flag.
		snac.append<quint16>(0);
		account()->connection()->send(snac);
		return true;
	} else if(ev->type() == Authorization::Request::eventType()) {
		debug() << "Handle auth request";
		Authorization::Request *request = static_cast<Authorization::Request*>(ev);
		SNAC snac(ListsFamily, ListsRequestAuth);
		snac.append<quint8>(d->uin);
		snac.append<quint16>(request->body());
		snac.append<quint16>(0);
		account()->connection()->send(snac);
		return true;
	}
	return Contact::event(ev);
}

void IcqContact::infoReceived(bool ok)
{
	ShortInfoMetaRequest *req = qobject_cast<ShortInfoMetaRequest*>(sender());
	Q_ASSERT(req);
	if (ok) {
		QString name = req->value(Nick, QString());
		if (!name.isEmpty())
			setName(name);
	}
	req->deleteLater();
}

} } // namespace qutim_sdk_0_3::oscar

