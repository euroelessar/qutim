/****************************************************************************
 *  icqcontact.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "icqcontact_p.h"
#include "messages.h"
#include "buddycaps.h"
#include "icqaccount.h"
#include "metainfo.h"
#include "qutim/messagesession.h"
#include "qutim/notificationslayer.h"
#include "qutim/messagesession.h"
#include <QApplication>

namespace qutim_sdk_0_3 {

namespace oscar {

void IcqContactPrivate::clearCapabilities()
{
	flags = 0;
	capabilities.clear();
}

void IcqContactPrivate::requestNick()
{
	Q_Q(IcqContact);
	if (name.isEmpty()) {
		ShortInfoMetaRequest *req = new ShortInfoMetaRequest(account, q);
		QObject::connect(req, SIGNAL(done(bool)), q, SLOT(infoReceived(bool)));
		req->send();
	}
}

FeedbagItem IcqContactPrivate::getNotInListGroup()
{
	const QString groupNameStr = QT_TRANSLATE_NOOP("ContactList", "General");
	FeedbagItem group = account->feedbag()->groupItem(not_in_list_group, Feedbag::CreateItem);
	if (!group.isInList()) {
		QString groupName(groupNameStr);
		for (int i = 1;; ++i) {
			if (!account->feedbag()->containsItem(SsiGroup, groupName)) {
				group.setName(groupName);
				break;
			}
			groupName = QString("%1 %2").arg(groupNameStr).arg(i);
		}
		group.update();
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
	d->status = Status::Offline;
	d->clearCapabilities();
}

IcqContact::~IcqContact()
{

}

QSet<QString> IcqContact::tags() const
{
	Q_D(const IcqContact);
	QSet<QString> groups;
	foreach (const FeedbagItem &item, d->items) {
		FeedbagItem group = d->account->feedbag()->groupItem(item.groupId());
		if (!group.isNull() && group.groupId() != not_in_list_group)
			groups << group.name();
	}
	foreach (const QString &tag, d->tags)
		groups.insert(tag);
	return groups;
}

QString IcqContact::id() const
{
	Q_D(const IcqContact);
	return d->uin;
}

QString IcqContact::name() const
{
	Q_D(const IcqContact);
	if (!d->name.isEmpty())
		return d->name;
	else
		return d->uin;
}

Status IcqContact::status() const
{
	Q_D(const IcqContact);
	return d->status;
}

QString IcqContact::avatar() const
{
	Q_D(const IcqContact);
	return d->avatar;
}

bool IcqContact::isInList() const
{
	Q_D(const IcqContact);
	return !d->items.isEmpty();
}

void IcqContact::sendMessage(const Message &message)
{
	Q_D(IcqContact);
	QString msgText;
	quint8 channel = 2;
	Cookie cookie(this, message.id());
	if (HtmlSupport())
		msgText = message.property("html").toString();
	if (msgText.isEmpty())
		msgText = message.text();
	if (!SrvRelaySupport()) {
		ServerMessage msgData(this, Channel1MessageData(msgText, CodecUtf16Be), cookie);
		d->account->connection()->send(msgData);
		channel = 1;
	} else {
		QTextCodec *codec;
		if (Utf8Support())
			codec = Util::utf8Codec();
		else
			codec = Util::asciiCodec();
		QByteArray msg = codec->fromUnicode(msgText) + '\0';
		Tlv2711 tlv(0x01, 0, d->status.subtype(), 1, cookie);
		tlv.append<quint16>(msg, LittleEndian);
		tlv.appendColors();
		if (Utf8Support())
			tlv.append<quint32>(ICQ_CAPABILITY_UTF8.toString().toUpper(), LittleEndian);
		ServerMessage msgData(this, Channel2MessageData(0, tlv));
		cookie.lock(this, SLOT(messageTimeout()));
		d->account->connection()->send(msgData);
	}
	debug().nospace() << "Message is sent on channel " << channel
			<< ", ID:" << message.id()
			<< ", text:" << message.text();
}

void IcqContact::setName(const QString &name)
{
	Q_D(IcqContact);
	d->name = name;
	d->requestNick(); // requestNick will do nothing if the name is not empty
	foreach (FeedbagItem item, d->items) {
		if (!name.isEmpty())
			item.setField(SsiBuddyNick, name);
		else
			item.removeField(SsiBuddyNick);
		item.update();
	}
}

void IcqContact::setTags(const QSet<QString> &tags)
{
	Q_D(IcqContact);
	if (!isInList())
		return;
	Feedbag *f = d->account->feedbag();
	f->beginModify();
	QHash<quint16, FeedbagItem> removeItems;
	foreach (const FeedbagItem &item, d->items)
		removeItems.insert(item.groupId(), item);
	QString name;
	DataUnit tagsData;
	bool first = true;
	foreach (const QString &tag, tags) {
		if (first) {
			name = tag;
			first = false;
		} else {
			FeedbagItem groupItem = f->groupItem(tag);
			if (groupItem.isInList())
				removeItems.remove(groupItem.groupId());
		}
		tagsData.append<quint16>(tag);
	}
	FeedbagItem tagsItem = f->item(SsiTags, id(), 0, Feedbag::GenerateId);
	tagsItem.setField(SsiBuddyTags, tagsData);
	tagsItem.update();
	FeedbagItem item = d->items.first();
	FeedbagItem newGroup = f->groupItem(name, Feedbag::GenerateId);
	FeedbagItem currentGroup = f->groupItem(item.groupId());
	removeItems.remove(newGroup.groupId());
	if (newGroup.groupId() != currentGroup.groupId()) {
		if (!newGroup.isInList())
			newGroup.update();
		FeedbagItem newItem = f->item(SsiBuddy, id(), newGroup.groupId(), Feedbag::GenerateId);
		newItem.setData(item.constData());
		newItem.update();
	} else {
		removeItems.remove(currentGroup.groupId());
	}
	foreach (FeedbagItem item, removeItems) {
		if (f->group(item.groupId()).count() <= 1) {
			f->removeItem(SsiGroup, item.groupId());
		} else {
			item.remove();
		}
	}
	f->endModify();
#if 0
	Feedbag *f = d->account->feedbag();
	f->beginModify();
	if (tags.isEmpty()) {
		FeedbagItem item = f->item(SsiBuddy, id(), d->getNotInListGroup().groupId(),
								   Feedbag::GenerateId | Feedbag::DontLoadLocal);
		item.setData(d->items.first().constData());
		item.update();
		foreach (FeedbagItem item, d->items)
			item.remove();
	} else {
		QHash<quint16, FeedbagItem> removeList;
		foreach (const FeedbagItem &item, d->items)
			removeList.insert(item.groupId(), item);
		foreach (const QString &name, tags) {
			FeedbagItem group = f->groupItem(name, Feedbag::GenerateId);
			if (!group.isInList())
				group.update();
			FeedbagItem item = removeList.take(group.groupId());
			if (item.isNull()) {
				item = f->item(SsiBuddy, id(), group.groupId(), Feedbag::GenerateId | Feedbag::DontLoadLocal);
				item.setData(d->items.first().constData());
				item.update();
			}
		}
		foreach (FeedbagItem item, removeList) {
			if (f->group(item.groupId()).count() <= 1) {
				f->removeItem(SsiGroup, item.groupId());
			} else {
				item.remove();
			}
		}
	}
	f->endModify();
#endif
}

void IcqContact::setInList(bool inList)
{
	Q_D(IcqContact);
	if (inList == isInList())
		return;
	if (inList) {
		Feedbag *f = d->account->feedbag();
		f->beginModify();
		FeedbagItem item;
		item = f->item(SsiBuddy, id(), d->getNotInListGroup().groupId(),
					   Feedbag::GenerateId | Feedbag::DontLoadLocal);
		item.setField<QString>(SsiBuddyNick, id());
		item.setField(SsiBuddyReqAuth);
		item.update();
		f->endModify();
	} else {
		foreach (FeedbagItem item, d->items)
			item.remove();
	}
}

void IcqContact::authResponse(const QString &message, bool auth)
{
	SNAC snac(ListsFamily, ListsCliAuthResponse);
	snac.append<qint8>(id()); // uin.
	snac.append<qint8>(auth ? 0x01 : 0x00); // auth flag.
	snac.append<qint16>(message);
	account()->connection()->send(snac);
}

void IcqContact::authRequest(const QString &message)
{
	SNAC snac(ListsFamily, ListsRequestAuth);
	snac.append<qint8>(id()); // uin.
	snac.append<qint16>(message);
	snac.append<quint16>(0);
	account()->connection()->send(snac);
}

IcqAccount *IcqContact::account()
{
	return d_func()->account;
}

bool IcqContact::RtfSupport() const
{
	return d_func()->flags & rtf_support;
}

bool IcqContact::HtmlSupport() const
{
	return d_func()->flags & html_support;
}

bool IcqContact::TypingSupport() const
{
	return d_func()->flags & typing_support;
}

bool IcqContact::AimChatSupport() const
{
	return d_func()->flags & aim_chat_support;
}

bool IcqContact::AimImageSupport() const
{
	return d_func()->flags & aim_image_support;
}

bool IcqContact::XtrazSupport() const
{
	return d_func()->flags & xtraz_support;
}

bool IcqContact::Utf8Support() const
{
	return d_func()->flags & utf8_support;
}

bool IcqContact::SendFileSupport() const
{
	return d_func()->flags & sendfile_support;
}

bool IcqContact::DirectSupport() const
{
	return d_func()->flags & direct_support;
}

bool IcqContact::IconSupport() const
{
	return d_func()->flags & icon_support;
}

bool IcqContact::GetFileSupport() const
{
	return d_func()->flags & getfile_support;
}

bool IcqContact::SrvRelaySupport() const
{
	return d_func()->flags & srvrelay_support;
}

bool IcqContact::AvatarSupport() const
{
	return d_func()->flags & avatar_support;
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

void IcqContact::setStatus(Status status)
{
	Q_D(IcqContact);
	d->status = status;
	if (status == Status::Offline)
		d->clearCapabilities();
	emit statusChanged(status);
}

void IcqContact::setCapabilities(const Capabilities &caps)
{
	Q_D(IcqContact);
	d->clearCapabilities();
	foreach(const Capability &capability, caps)
	{
		if (capability.match(ICQ_CAPABILITY_RTFxMSGS))
			d->flags |= rtf_support;
		else if (capability.match(ICQ_CAPABILITY_TYPING))
			d->flags |= typing_support;
		else if (capability.match(ICQ_CAPABILITY_AIMCHAT))
			d->flags |= aim_chat_support;
		else if (capability.match(ICQ_CAPABILITY_AIMIMAGE))
			d->flags |= aim_image_support;
		else if (capability.match(ICQ_CAPABILITY_XTRAZ))
			d->flags |= xtraz_support;
		else if (capability.match(ICQ_CAPABILITY_UTF8))
			d->flags |= utf8_support;
		else if (capability.match(ICQ_CAPABILITY_AIMSENDFILE))
			d->flags |= sendfile_support;
		else if (capability.match(ICQ_CAPABILITY_DIRECT))
			d->flags |= direct_support;
		else if (capability.match(ICQ_CAPABILITY_AIMICON))
			d->flags |= icon_support;
		else if (capability.match(ICQ_CAPABILITY_AIMGETFILE))
			d->flags |= getfile_support;
		else if (capability.match(ICQ_CAPABILITY_SRVxRELAY))
			d->flags |= srvrelay_support;
		else if (capability.match(ICQ_CAPABILITY_AVATAR))
			d->flags |= avatar_support;
	}
	d->capabilities = caps;
}

bool IcqContact::event(QEvent *ev)
{
	Q_D(IcqContact);
	if (ev->type() == ChatStateEvent::eventType()) {
		ChatStateEvent *chatEvent = static_cast<ChatStateEvent *>(ev);
		ChatState state = chatEvent->chatState();
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
			return true;
		SNAC sn(MessageFamily, MessageMtn);
		sn.append(Cookie(true));
		sn.append<quint16>(1); // channel?
		sn.append<quint8>(d->uin);
		sn.append<quint16>(type);
		d->account->connection()->send(sn);
		return true;
	}
	return Contact::event(ev);
}

void IcqContact::messageTimeout()
{
	Cookie *cookie = qobject_cast<Cookie*>(sender());
	Q_ASSERT(cookie);
	ChatSession *session = ChatLayer::instance()->getSession(cookie->contact(), false);
	if (session) {
		QApplication::instance()->postEvent(session, new MessageReceiptEvent(cookie->id(), false));
		debug() << "Message with id" << cookie->id() << "has not been delivered";
	}
}

void IcqContact::infoReceived(bool ok)
{
	ShortInfoMetaRequest *req = qobject_cast<ShortInfoMetaRequest*>(sender());
	Q_ASSERT(req);
	if (ok) {
		QString name = req->value<QString>(Nick);
		if (!name.isEmpty())
			setName(name);
	}
	req->deleteLater();
}

} } // namespace qutim_sdk_0_3::oscar
