/****************************************************************************
 *  icqcontact.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
#include "roster.h"
#include "messages.h"
#include "buddycaps.h"
#include "qutim/messagesession.h"
#include "qutim/notificationslayer.h"

namespace Icq
{

void IcqContactPrivate::clearCapabilities()
{
	flags = 0;
	capabilities.clear();
}

IcqContact::IcqContact(const QString &uin, IcqAccount *account) :
	Contact(account), d_ptr(new IcqContactPrivate)
{
	Q_D(IcqContact);
	d->account = account;
	d->uin = uin;
	d->status = Offline;
	d->clearCapabilities();
}

IcqContact::~IcqContact()
{

}

QSet<QString> IcqContact::tags() const
{
	Q_D(const IcqContact);
	QSet<QString> group;
	QString group_name = d->group_id == not_in_list_group ? QString() : d->account->roster()->groupId2Name(d->group_id);
	if (!group_name.isNull())
		group.insert(group_name);
	return group;
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

bool IcqContact::isInList() const
{
	Q_D(const IcqContact);
	return d->group_id == not_in_list_group;
}

void IcqContact::sendMessage(const Message &message)
{
	Q_D(IcqContact);
	QString msgText;
	if (HtmlSupport())
		msgText = message.property("html").toString();
	if (msgText.isEmpty())
		msgText = message.text();
	if (!SrvRelaySupport()) {
		ServerMessage msgData(d->uin, Channel1MessageData(msgText, CodecUtf16Be));
		d->account->connection()->send(msgData);
		debug() << "Message" << msgText << "is sent on channel 1";
	} else {
		QTextCodec *codec;
		if (Utf8Support())
			codec = Util::utf8Codec();
		else
			codec = Util::asciiCodec();
		QByteArray msg = codec->fromUnicode(msgText) + '\0';
		Tlv2711 tlv(0x01, 0, qutimStatusToICQ(d->status), 1);
		tlv.appendData<quint16> (msg, LittleEndian);
		tlv.appendColors();
		if (Utf8Support())
			tlv.appendData<quint32> (ICQ_CAPABILITY_UTF8.toString().toUpper(), LittleEndian);
		ServerMessage msgData(d->uin, Channel2MessageData(0, tlv));
		d->account->connection()->send(msgData);
		debug() << "Message" << msgText << "is sent on channel 2";
	}
}

void IcqContact::setName(const QString &name)
{
	Q_D(IcqContact);
	d->account->roster()->sendRenameContactRequest(d->uin, name);
}

void IcqContact::setTags(const QSet<QString> &tags)
{
}

void IcqContact::setInList(bool inList)
{
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

void IcqContact::setStatus(Status status)
{
	Q_D(IcqContact);
	d->status = status;
	if (status == Offline)
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
	if (ev->type() == ChatStateEvent::eventType()) {
		ChatStateEvent *chatEvent = static_cast<ChatStateEvent *>(ev);
		ChatState state = chatEvent->chatState();
		Q_D(IcqContact);
		qint8 type = -1;
		if (state == ChatStatePaused)
			type = 1;
		else if (state == ChatStateComposing)
			type = 2;
		if (type < 0)
			return true;
		SNAC sn(MessageFamily, MessageMtn);
		sn.appendSimple<quint64> (Util::generateCookie());
		sn.appendSimple<quint16> (1); // ???
		sn.appendData<quint8> (d->uin);
		sn.appendSimple<quint16> (type);
		d->account->connection()->send(sn);
		return true;
	}
	return Contact::event(ev);
}

} // namespace Icq
