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
#include "metainfo/infometarequest.h"
#include "inforequest_p.h"
#include "qutim/messagesession.h"
#include "qutim/notificationslayer.h"
#include "qutim/tooltip.h"
#include "qutim/extensionicon.h"
#include <QApplication>
#include <QVariant>

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
	d->state = ChatStateGone;
}

IcqContact::~IcqContact()
{

}

QStringList IcqContact::tags() const
{
	Q_D(const IcqContact);
	QStringList groups = d->tags;
	foreach (const FeedbagItem &item, d->items) {
		FeedbagItem group = d->account->feedbag()->groupItem(item.groupId());
		if (!group.isNull() && group.groupId() != not_in_list_group && !d->tags.contains(group.name(), Qt::CaseSensitive))
			groups << group.name();
	}
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

/*
 Looks up an incomplete character at the end of a utf-8 string and returns
 length of this character or 0 if the character has not been found.
 */
static quint8 lookupIncompleteCharacter(const QByteArray &str)
{
	int left = str.size(); // holds count of remained bytes
	int i = 0;
	const uchar *data = reinterpret_cast<const uchar *>(str.data());
	while (left > 0) {
		const uchar current = data[i];
		if (current < 0xc2) {
			--left;
			++i;
		} else if (current < 0xe0) {
			left -= 2;
			i += 2;
		} else /*if (current < 0xf0)*/ {
			left -= 3;
			i += 3;
		}
	}
	return -left;
}

enum SplitFlags
{
	sf_utf8 = 0x01,
	sf_appendNull = 0x02
};

/*
 Splits the long message into messages with length less than maxLen and returns the list of
 those messages. If the length of this message less than maxLen, returns a single-element
 list containing this message.
 */
static QList<QByteArray> splitMessage(const QByteArray &message, quint16 maxLen, quint8 flags = 0)
{
	QList<QByteArray> list;
	if (message.size() > maxLen) {
		int i = 0;
		int size = message.size();
		while (i < size) {
			QByteArray msg = message.mid(i, maxLen);
			if (flags & sf_utf8) {
				quint8 l = lookupIncompleteCharacter(msg);
				if (l)
					msg.truncate(maxLen-l);
			}
			i += msg.size();
			if (flags & sf_appendNull)
				msg += '\0';
			list << msg;
		}
	} else {
		if (flags & sf_appendNull)
			list << (message + '\0');
		else
			list <<  message;
	}
	return list;
}

bool IcqContact::sendMessage(const Message &message)
{
	Q_D(IcqContact);

	if (account()->status() == Status::Offline)
		return false;

	QString msgText;
	quint8 channel = 2;
	Cookie cookie(this, message.id());
	if (d->flags & html_support)
		msgText = message.property("html").toString();
	if (msgText.isEmpty())
		msgText = message.text();
	if (!(d->flags & srvrelay_support)) {
		Channel1Codec charset = CodecUtf16Be;
		QByteArray data = Channel1MessageData::fromUnicode(msgText, charset);
		QList<QByteArray> msgs = splitMessage(data, 2542); // Max: 2543

		for (int i = 0, size = msgs.size(), last = size-1; i < size; ++i) {
			ServerMessage msgData(this, Channel1MessageData(msgs.at(i), charset),
								  i == last ? cookie : Cookie::generateId());
			d->account->connection()->send(msgData, 80);
		}
		channel = 1;
	} else {
		QList<QByteArray> msgs = d->flags & utf8_support ?
								 splitMessage(Util::utf8Codec()->fromUnicode(msgText), 7857, sf_utf8 | sf_appendNull) :
								 splitMessage(Util::asciiCodec()->fromUnicode(msgText), 7898, sf_appendNull);

		for (int i = 0, size = msgs.size(), last = size-1; i < size; ++i) {
			bool isLast = i == last;
			const QByteArray &msg = msgs.at(i);
			Tlv2711 tlv(0x01, 0, d->status.subtype(), 1,
						isLast ? cookie : Cookie::generateId());
			tlv.append<quint16>(msg, LittleEndian);
			tlv.appendColors();
			if (d->flags & utf8_support)
				tlv.append<quint32>(ICQ_CAPABILITY_UTF8.toString().toUpper(), LittleEndian);
			ServerMessage msgData(this, Channel2MessageData(0, tlv));
			if (isLast)
				msgData.setCookie(cookie, this, SLOT(messageTimeout(Cookie)));
			d->account->connection()->send(msgData, 80);
		}
	}
	debug(Verbose).nospace()
			<< "Message is sent on channel " << channel
			<< ", ID:" << message.id()
			<< ", text:" << message.text();
	return true;
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

void IcqContact::setTags(const QStringList &tags)
{
	Q_D(IcqContact);
	if (!isInList())
		return;
	Feedbag *f = d->account->feedbag();
	f->beginModify();
	setGroup(tags.isEmpty() ? QString() : tags.first());
	DataUnit tagsData;
	foreach (const QString &tag, tags)
		tagsData.append<quint16>(tag);
	FeedbagItem tagsItem = f->item(SsiTags, id(), 0, Feedbag::GenerateId);
	tagsItem.setField(SsiBuddyTags, tagsData);
	tagsItem.update();
	f->endModify();
}

void IcqContact::setGroup(const QString &group)
{
	Q_D(IcqContact);
	Feedbag *f = d->account->feedbag();
	bool modify = !f->isModifyStarted();
	if (modify)
		f->beginModify();
	bool found = false;
	QList<FeedbagItem> items = d->items;
	FeedbagItem newGroup;
	if (group.isEmpty())
		newGroup = d->getNotInListGroup();
	else
		newGroup = f->groupItem(group, Feedbag::GenerateId);
	if (newGroup.isInList()) {
		QList<FeedbagItem>::iterator itr = items.begin();
		QList<FeedbagItem>::iterator endItr = items.end();
		while (itr != endItr) {
			if (itr->groupId() == newGroup.groupId()) {
				// The contact is already contained in the group.
				found = true;
				items.erase(itr); // This item should not be removed from server.
				break;
			}
			++itr;
		}
	} else {
		newGroup.update();
	}
	if (!found) {
		// Copy the contact to the group.
		FeedbagItem newItem = f->item(SsiBuddy, id(), newGroup.groupId(), Feedbag::GenerateId);
		newItem.setData(d->items.first().constData());
		newItem.update();
	}
	// Remove unnecessary items
	foreach (FeedbagItem item, items) {
		item.remove();
		if (f->group(item.groupId()).count() <= 1)
			f->removeItem(SsiGroup, item.groupId());
	}
	if (modify)
		f->endModify();
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
		item.update();
		f->endModify();
	} else {
		foreach (FeedbagItem item, d->items)
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

void IcqContact::setStatus(Status status)
{
	Q_D(IcqContact);
	Status previous = d->status;
	d->status = status;
	if (status == Status::Offline) {
		d->clearCapabilities();
		d->onlineSince = QDateTime();
		d->awaySince = QDateTime();
		d->regTime = QDateTime();
	}
	emit statusChanged(status, previous);
}

ChatState IcqContact::chatState() const
{
	return d_func()->state;
}

QList<FeedbagItem> IcqContact::feedbagItems() const
{
	return d_func()->items;
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
		d->account->connection()->send(sn, 20);
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
	} else if (ev->type() == InfoRequestCheckSupportEvent::eventType()) {
		Status::Type status = account()->status().type();
		if (status >= Status::Online && status <= Status::Invisible) {
			InfoRequestCheckSupportEvent *event = static_cast<InfoRequestCheckSupportEvent*>(ev);
			event->setSupportType(InfoRequestCheckSupportEvent::Read);
			event->accept();
		} else {
			ev->ignore();
		}
	} else if (ev->type() == InfoRequestEvent::eventType()) {
		InfoRequestEvent *event = static_cast<InfoRequestEvent*>(ev);
		event->setRequest(new IcqInfoRequest(this));
		event->accept();
	}
	return Contact::event(ev);
}

void IcqContact::messageTimeout(const Cookie &cookie)
{
	ChatSession *session = ChatLayer::instance()->get(cookie.contact(), false);
	if (session) {
		QApplication::instance()->postEvent(session, new MessageReceiptEvent(cookie.id(), false));
		debug() << "Message with id" << cookie.id() << "has not been delivered";
	}
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
