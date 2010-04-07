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
#include "qutim/tooltip.h"
#include "qutim/extensionicon.h"
#include <QApplication>
#include <QVariant>

Q_DECLARE_METATYPE(QHostAddress);

namespace qutim_sdk_0_3 {

namespace oscar {

IcqInfoRequest::IcqInfoRequest(IcqContact *contact) :
	m_metaReq(new FullInfoMetaRequest(contact->account(), contact)),
	m_state(Request),
	m_contact(contact)
{
	connect(m_metaReq, SIGNAL(done(bool)), SLOT(onDone(bool)));
	m_metaReq->send();
}

IcqInfoRequest::~IcqInfoRequest()
{
	if (m_metaReq)
		m_metaReq->deleteLater();
}

InfoItem IcqInfoRequest::item(const QString &name) const
{
	if (name.isEmpty()) {
		QList<InfoItem> groups;
		QString lastName;
		for (int i = FirstMetaField; i <= LastMetaField; ++i) {
			MetaInfoField field(i);
			LocalizedString groupName = field.group();
			if (lastName != groupName) {
				groups.push_back(InfoItem(groupName.original(), groupName, QVariant()));
				lastName = groupName.original();
			}
			InfoItem item(field.name(), field.toString(), m_values.value(field));
			LocalizedStringList alt = field.titleAlternatives();
			if (!alt.isEmpty())
				item.setProperty("titleAlternatives", QVariant::fromValue(alt));
			alt = field.alternatives();
			if (!alt.isEmpty())
				item.setProperty("alternatives", QVariant::fromValue(alt));
			groups.last().addSubitem(item);
		}
		InfoItem item;
		foreach (const InfoItem &i, groups)
			item.addSubitem(i);
		InfoItem otherGroup(QT_TRANSLATE_NOOP("ContactInfo", "Other"));
		DirectConnectionInfo info = m_contact->dcInfo();
		otherGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Internal IP"), QVariant::fromValue(info.internal_ip)));
		otherGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "External IP"), QVariant::fromValue(info.external_ip)));
		otherGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Port"), info.port));
		otherGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Protocol version"), info.protocol_version));
		{
			QStringList caps;
			foreach (const Capability &cap, m_contact->capabilities())
				caps << cap.name();
			otherGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Capabilities"), caps));
		}
		{
			InfoItem dcGroup(QT_TRANSLATE_NOOP("ContactInfo", "Direct connection extra info"));
			dcGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Last info update"),
										   QString("0x%1").arg(info.info_utime, 0, 16)));
			dcGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Last ext info update"),
										   QString("0x%1").arg(info.extinfo_utime, 0, 16)));
			dcGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Last ext status update"),
										   QString("0x%1").arg(info.extstatus_utime, 0, 16)));
			dcGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Client features"),
										   QString("0x%1").arg(info.client_features, 0, 16)));
			dcGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Cookie"), info.auth_cookie));
			otherGroup.addSubitem(dcGroup);
		}
		item.addSubitem(otherGroup);
		return item;
	} else {
		MetaInfoField field(name);
		return InfoItem(name, field.toString(), m_values.value(field));
	}
}

IcqInfoRequest::State IcqInfoRequest::state() const
{
	return m_state;
}

void IcqInfoRequest::onDone(bool ok)
{
	Q_ASSERT(sender());
	if (m_metaReq != sender())
		return;
	if (ok) {
		m_values = m_metaReq->values();
		m_state = Done;
	} else {
		m_state = Cancel;
	}
	emit stateChanged(m_state);
	m_metaReq->deleteLater();
}

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

void IcqContactPrivate::setChatState(ChatState state)
{
	Q_Q(IcqContact);
	state = state;
	qApp->postEvent(ChatLayer::get(q, true), new ChatStateEvent(state));
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
	d->added = false;
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
	if (d->flags & html_support)
		msgText = message.property("html").toString();
	if (msgText.isEmpty())
		msgText = message.text();
	if (!(d->flags & srvrelay_support)) {
		ServerMessage msgData(this, Channel1MessageData(msgText, CodecUtf16Be), cookie);
		d->account->connection()->send(msgData);
		channel = 1;
	} else {
		QTextCodec *codec;
		if (d->flags & utf8_support)
			codec = Util::utf8Codec();
		else
			codec = Util::asciiCodec();
		QByteArray msg = codec->fromUnicode(msgText) + '\0';
		Tlv2711 tlv(0x01, 0, d->status.subtype(), 1, cookie);
		tlv.append<quint16>(msg, LittleEndian);
		tlv.appendColors();
		if (d->flags & utf8_support)
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
	FeedbagItem group = f->groupItem(d->items.first().groupId());
	if (!group.isNull() && !tags.contains(group.name()))
		setGroup(tags.isEmpty() ? QString() : tags.toList().first());
	DataUnit tagsData;
	foreach (const QString &tag, tags)
		tagsData.append<quint16>(tag);
	FeedbagItem tagsItem = f->item(SsiTags, id(), 0, Feedbag::GenerateId);
	tagsItem.setField(SsiBuddyTags, tagsData);
	tagsItem.update();
}

void IcqContact::setGroup(const QString &group)
{
	Q_D(IcqContact);
	Feedbag *f = d->account->feedbag();
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

InfoRequest *IcqContact::infoRequest() const
{
	return new IcqInfoRequest(const_cast<IcqContact*>(this));
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
	d->status = status;
	if (status == Status::Offline) {
		d->clearCapabilities();
		d->onlineSince = QDateTime();
		d->awaySince = QDateTime();
		d->regTime = QDateTime();
	}
	emit statusChanged(status);
}

ChatState IcqContact::chatState() const
{
	return d_func()->state;
}

void IcqContact::insertToolTipField(const LocalizedString &title, const QVariant &data)
{
	d_func()->fields.insert(title.original(), InfoField(title, data));
}

void IcqContact::removeToolTipField(const QString &title)
{
	d_func()->fields.remove(title);
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
	} else if (ev->type() == ToolTipEvent::eventType()) {
		ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
		QVariantHash extStatuses = status().property("extendedStatuses", QVariantHash());
		if (!extStatuses.isEmpty()) {
			foreach (const QVariant &itr, extStatuses) {
				QVariantHash extStatus = itr.toHash();
				event->appendField(extStatus.value("title").toString(),
								   extStatus.value("desc").toString(),
								   extStatus.value("icon").value<ExtensionIcon>());
			}
		} else if (!status().text().isEmpty()) {
			event->appendField(QString(), status().text());
		}
		QDateTime time;
		if (!d->onlineSince.isNull()) {
			time = QDateTime::currentDateTime();
			time = time.addSecs(-d->onlineSince.toTime_t());
			time = time.toUTC();
			event->appendField(QT_TRANSLATE_NOOP("ContactList", "Online time"),
							   QString("%1d %2h %3m %4s")
							   .arg(time.date().day() - 1)
							   .arg(time.time().hour())
							   .arg(time.time().minute())
							   .arg(time.time().second()));
			event->appendField(QT_TRANSLATE_NOOP("ContactList", "Signed on"),
							   d->onlineSince.toLocalTime().toString("hh:mm:ss dd/MM/yyyy"));
		}
		if (!d->awaySince.isNull()) {
			event->appendField(QT_TRANSLATE_NOOP("ContactList", "Away since"),
							   d->awaySince.toLocalTime().toString("hh:mm:ss dd/MM/yyyy"));
		}
		if (!d->regTime.isNull()) {
			event->appendField(QT_TRANSLATE_NOOP("ContactList", "Reg. date"),
							   d->regTime.toLocalTime().toString("hh:mm:ss dd/MM/yyyy"));
		}
		foreach (const InfoField &field, d->fields) {
			event->appendField(field.first, field.second);
		}
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
