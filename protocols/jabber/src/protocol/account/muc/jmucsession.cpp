/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "jmucsession.h"
#include "jmucuser.h"
#include "../jaccount.h"
#include "../roster/jmessagesession.h"
#include "../roster/jmessagehandler.h"
#include "../../jprotocol.h"
#include "jbookmarkmanager.h"
#include <qutim/message.h>
#include <jreen/message.h>
#include <qutim/chatsession.h>
#include <jreen/vcardupdate.h>
#include <jreen/delayeddelivery.h>
#include "../../../jabber_global.h"
#include <QStringBuilder>
#include <QMessageBox>
#include <QFile>
#include "jmucmanager.h"
#include "jconferenceconfig.h"
#include "../roster/jcontactresource_p.h"
#include <jreen/client.h>
#include "../roster/jsoftwaredetection.h"
#include "../dataform/jdataform.h"
#include <jreen/captcha.h>
#include <qutim/systemintegration.h>
#include <QVBoxLayout>
#include <qutim/notification.h>
#include <QApplication>
#include <qutim/debug.h>
#include <QInputDialog>
#include <QLabel>
#include <QTimer>

using namespace Jreen;
using namespace qutim_sdk_0_3;

namespace Jabber
{
class JMUCSessionPrivate
{
public:
	void removeUser(JMUCSession *session, JMUCUser *user);
	JMUCUser *addUser(JMUCSession *session, const QString &nick);
	JMUCUser *getUser(const QString &nick);
	bool containsUser(const QString &nick);
	
	QWeakPointer<JAccount> account;
	QList<Jreen::MessageFilter*> filters;
	Jreen::MUCRoom *room;
	QWeakPointer<QWidget> captchaForm;
	Jreen::JID jid;
	QString title;
	QString topic;
	QHash<QString, quint64> messages;
	QHash<QString, JMUCUser *> users;
	bool isAutoRejoin;
	Jreen::Bookmark::Conference bookmark;
	QWeakPointer<JConferenceConfig> config;
	bool avatarsAutoLoad;
	bool isError;
	QDateTime lastMessage;
	QString *thread;
};

void JMUCSessionPrivate::removeUser(JMUCSession *conference, JMUCUser *user)
{
	if (ChatSession *session = ChatLayer::get(conference, false))
		session->removeContact(user);
	
	if (ChatSession *session = ChatLayer::get(user, false)) {
		QObject::connect(session, SIGNAL(destroyed()), user, SLOT(deleteLater()));
	} else {
		users.remove(user->name());
		user->deleteLater();
	}
}

JMUCUser *JMUCSessionPrivate::addUser(JMUCSession *conference, const QString &nick)
{
	JMUCUser *user = users.value(nick);
	if (!user) {
		user = new JMUCUser(conference, nick);
		users.insert(nick, user);
	} else if (ChatSession *session = ChatLayer::get(conference, false)) {
		QObject::disconnect(session, SIGNAL(destroyed()), user, SLOT(deleteLater()));
	}
	return user;
}

JMUCUser *JMUCSessionPrivate::getUser(const QString &nick)
{
	JMUCUser *user = users.value(nick);
	if (user && user->presenceType() != Presence::Unavailable)
		return user;
	return 0;
}

bool JMUCSessionPrivate::containsUser(const QString &nick)
{
	JMUCUser *user = users.value(nick);
	return (user && user->presenceType() != Presence::Unavailable);
}

JMUCSession::JMUCSession(const Jreen::JID &room, const QString &password, JAccount *account) :
	Conference(account), d_ptr(new JMUCSessionPrivate)
{
	Q_D(JMUCSession);
	d->jid = room.bareJID();
	d->account = account;
	d->room = new Jreen::MUCRoom(account->client(), room);
	d->room->setPassword(password);
	d->filters << new JMessageReceiptFilter(account, 0);
	connect(d->room, SIGNAL(presenceReceived(Jreen::Presence,const Jreen::MUCRoom::Participant*)),
			this, SLOT(onParticipantPresence(Jreen::Presence,const Jreen::MUCRoom::Participant*)));
	connect(d->room, SIGNAL(presenceReceived(Jreen::Presence,const Jreen::MUCRoom::Participant*)),
			d->account.data()->softwareDetection(), SLOT(handlePresence(Jreen::Presence)));
	connect(d->room, SIGNAL(messageReceived(Jreen::Message,bool)),
			this, SLOT(onMessage(Jreen::Message,bool)));
	connect(d->room, SIGNAL(serviceMessageReceived(Jreen::Message)),
			this, SLOT(onServiceMessage(Jreen::Message)));
	connect(d->room, SIGNAL(subjectChanged(QString,QString)),
			this, SLOT(onSubjectChanged(QString,QString)));
	connect(d->room, SIGNAL(leaved()), this, SLOT(joinedChanged()));
	connect(d->room, SIGNAL(joined()), this, SLOT(joinedChanged()));
	connect(d->room, SIGNAL(error(Jreen::Error::Ptr)),
			this, SLOT(onError(Jreen::Error::Ptr)));
	//	if (!password.isEmpty())
	//		d->room->setPassword(password);
	d->isError = false;
	d->thread = 0;
	d->title = room.bare();
	loadSettings();
}

//JMUCSession::JMUCSession(JAccount *account, gloox::MUCRoom *room, const std::string &thread) :
//	Conference(account), d_ptr(new JMUCSessionPrivate)
//{
//	Q_D(JMUCSession);
//	d->account = account;
//	d->room = room;
//	d->jid.setServer(room->service());
//	d->jid.setUsername(room->name());
//	d->nick = QString::fromStdString(room->nick());
//	d->thread = new std::string(thread);
//	d->isJoined = true;
//	d->isConfiguring = false;
//	d->isError = false;
//	d->account->conferenceManager()->appendMUCSession(this);
//	room->registerMUCRoomHandler(this);
//	room->registerMUCRoomConfigHandler(this);
//}

JMUCSession::~JMUCSession()
{
	Q_D(JMUCSession);
	if (d->account)
		d->room->leave();
	foreach (Jreen::MessageFilter *filter, d->filters)
		delete filter;
}

qutim_sdk_0_3::Buddy *JMUCSession::me() const
{
	Q_D(const JMUCSession);
	return d->users.value(d->room->nick());
}

ChatUnit *JMUCSession::participant(const QString &nick)
{
	return d_func()->users.value(nick);
}

QString JMUCSession::nick()
{
	return d_func()->room->nick();
}

void JMUCSession::setNick(const QString &nick)
{
	Q_D(JMUCSession);
	d->room->setNick(nick);
	if (!d->room->isJoined())
		emit nickChanged(nick);
}

void JMUCSession::doJoin()
{
	Q_D(JMUCSession);
	if(isJoined() || !d->account.data()->client()->isConnected())
		return;
	d->room->join();
	emit joined();
}

void JMUCSession::doLeave()
{
	Q_D(JMUCSession);
	if(!isJoined())
		return;
	d->room->leave();
	//remove users
	const Presence presence(Presence::Unavailable, JID());
	foreach (JMUCUser *user, d->users) {
		if (user->presenceType() == Presence::Unavailable)
			continue;
		user->setStatus(presence);
		d->removeUser(this, user);
	}
	emit left();
}

void JMUCSession::kick(const QString &nick, const QString &reason)
{
	d_func()->room->kick(nick, reason);
}

void JMUCSession::ban(const QString &nick, const QString &reason)
{
	d_func()->room->ban(nick, reason);
}

void JMUCSession::unban(const QString &jid, const QString &reason)
{
	d_func()->room->setAffiliation(JID(jid), MUCRoom::AffiliationNone, reason);
}

void JMUCSession::member(const QString &nick, const QString &reason)
{
	d_func()->room->setAffiliation(nick, MUCRoom::AffiliationMember, reason);
}

void JMUCSession::voice(const QString &nick, const QString &reason)
{
	d_func()->room->setRole(nick, MUCRoom::RoleParticipant, reason);
}

void JMUCSession::moder(const QString &nick, const QString &reason)
{
	d_func()->room->setRole(nick, MUCRoom::RoleModerator, reason);
}

void JMUCSession::visitor(const QString &nick, const QString &reason)
{
	d_func()->room->setRole(nick, MUCRoom::RoleVisitor, reason);
}

void JMUCSession::admin(const QString &nick, const QString &reason)
{
	d_func()->room->setAffiliation(nick, MUCRoom::AffiliationAdmin, reason);
}

void JMUCSession::owner(const QString &nick, const QString &reason)
{
	d_func()->room->setAffiliation(nick, MUCRoom::AffiliationOwner, reason);
}

QString JMUCSession::id() const
{
	Q_D(const JMUCSession);
	return d->room->id();
}

bool JMUCSession::sendMessage(const qutim_sdk_0_3::Message &message)
{
	Q_D(JMUCSession);
	debug() << d->jid;
	debug() << d->jid.bareJID();

	if (account()->status() == Status::Offline)
		return false;
	if (!isJoined())
		return false;

	if (message.text().startsWith("/nick ")) {
		QString nick = message.text().section(' ', 1);
		if (!nick.isEmpty()) {
			if (d->users.contains(nick))
				return false;
			d->room->setNick(nick);
		}
		return true;
	} else if (message.text().startsWith("/topic ")) {
		QString topic = message.text().section(' ',1);
		if (!topic.isEmpty()) {
			setTopic(topic);
			return true;
		}
	}
	Jreen::Message jMsg(Jreen::Message::Groupchat, d->jid.bareJID(), message.text());
	jMsg.setID(d->account.data()->client()->getID());
	d->messages.insert(jMsg.id(), message.id());
	d->account.data()->client()->send(jMsg);
	return true;
}

bool JMUCSession::sendPrivateMessage(JMUCUser *user, const qutim_sdk_0_3::Message &message)
{
	Q_D(JMUCSession);
	if (account()->status() == Status::Offline)
		return false;
	Jreen::Message msg(Jreen::Message::Chat,
					   user->id(),
					   message.text(),
					   message.property("subject").toString());
	msg.setID(QString::number(message.id()));
	foreach (Jreen::MessageFilter *filter, d->filters)
		filter->decorate(msg);
	d->account.data()->client()->send(msg);
	return true;
}

void JMUCSession::onParticipantPresence(const Jreen::Presence &presence,
										const Jreen::MUCRoom::Participant *participant)
{
	Q_D(JMUCSession);
	Notification::Type notificationType = Notification::System;
	QString nick = presence.from().resource();
	bool isSelf = nick == d->room->nick();
	QString text;
	if (participant->isBanned() || participant->isKicked()) {
		QString reason = participant->reason();
		bool isBan = participant->isBanned();
		text = nick % (isBan ? tr(" has been banned") : tr(" has been kicked"));
		if (!reason.isEmpty())
			text = text % " (" % reason % ")";
		if (isSelf) {
			QString msgtxt = (isBan ? tr("You has been banned at ") : tr("You has been kicked from ")) % id() % "\n";
			if (!reason.isEmpty())
				msgtxt = msgtxt % tr("with reason: ") % reason.append("\n");
			if (!isBan) {
				msgtxt = msgtxt % tr("Do you want to rejoin?");
				if (QMessageBox::warning(0, tr("You have been kicked"), msgtxt,
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {
					QTimer::singleShot(0, this, SLOT(join()));
				}
			} else {
				QMessageBox::warning(0, tr("You have been banned"), msgtxt, QMessageBox::Ok);
			}
		} else {
			JMUCUser *user = d->getUser(nick);
			user->setStatus(presence);
			d->removeUser(this, user);
		}
		notificationType = Notification::ChatUserLeft;
	} else if (participant->isNickChanged()) {
		QString newNick = participant->newNick();
		if (newNick.isEmpty())
			return;
		text = nick % tr(" is now known as ") % newNick;
		JMUCUser *user = d->users.value(nick, 0);
		if (!user) {
			warning() << "Nick change presence for unknown contact";
			return;
		}
		d->users.insert(newNick, user);
		d->users.remove(nick);
		user->setUserName(newNick);
		if (isSelf) {
			emit nickChanged(newNick);
			emit meChanged(me());
		}
	} else {
		JMUCUser *user = d->getUser(nick);
		if (!user && presence.subtype() != Presence::Unavailable) {
			user = d->addUser(this, nick);
			user->setStatus(presence);
			user->setMUCAffiliation(participant->affiliation());
			user->setMUCRole(participant->role());
			if (participant->realJID().isValid())
				user->setRealJid(participant->realJID());
			text = user->realJid().isEmpty()
					? nick
					: nick % QLatin1Literal(" (") % user->realJid() % QLatin1Literal(")");
			text = text % tr(" has joined the room");
			if (participant->affiliation() == MUCRoom::AffiliationOwner)
				text = text % tr(" as") % tr(" owner");
			else if (participant->affiliation() == MUCRoom::AffiliationAdmin)
				text = text % tr(" as") % tr(" administrator");
			else if (participant->affiliation() == MUCRoom::AffiliationMember)
				text = text % tr(" as") % tr(" registered member");
			else if (participant->role() == MUCRoom::RoleParticipant)
				text = text % tr(" as") % tr(" participant");
			else if (participant->role() == MUCRoom::RoleVisitor)
				text = text % tr(" as") % tr(" visitor");
			// addUser doesn't handle addition to the session
			if (ChatSession *session = ChatLayer::get(this, false))
				session->addContact(user);
			notificationType = Notification::ChatUserJoined;
		} else if (!user) {
			return;
		} else {
			user->setStatus(presence);
			if (presence.subtype() == Presence::Unavailable) {
				text = nick % tr(" has left the room");
				d->removeUser(this, user);
				notificationType = Notification::ChatUserLeft;
			}
		}
		if (presence.subtype() != Presence::Unavailable &&
				(user->role() != participant->role()
				 || user->affiliation() != participant->affiliation())) {
			text = user->name() % tr(" now is");
			if (participant->affiliation() == MUCRoom::AffiliationOwner) {
				text = text % tr(" owner");
			} else if (participant->affiliation() == MUCRoom::AffiliationAdmin) {
				text = text % tr(" administrator");
				if (participant->role() == MUCRoom::RoleParticipant)
					text = text % tr(" and") % tr(" participant");
				else if (participant->role() == MUCRoom::RoleVisitor)
					text = text % tr(" and") % tr(" visitor");
			} else if (participant->affiliation() == MUCRoom::AffiliationMember) {
				text = text % tr(" registered member");
				if (participant->role() == MUCRoom::RoleModerator)
					text = text % tr(" and") % tr(" moderator");
				else if (participant->role() == MUCRoom::RoleVisitor)
					text = text % tr(" and") % tr(" visitor");
			} else if (participant->role() == MUCRoom::RoleModerator) {
				text = text % tr(" moderator");
			} else if (participant->role() == MUCRoom::RoleParticipant) {
				text = text % tr(" participant");
			} else if (participant->role() == MUCRoom::RoleVisitor)  {
				text = text % tr(" visitor");
			}
			user->setMUCAffiliation(participant->affiliation());
			user->setMUCRole(participant->role());
		}
	}
	if (!text.isEmpty() && (isJoined() || participant->isKicked() || participant->isBanned())) {
		NotificationRequest request(notificationType);
		request.setObject(this);
		request.setText(text);
		request.setProperty("senderName", nick);
		request.send();
	}
}

void JMUCSession::onMessage(Jreen::Message msg, bool priv)
{
	Q_D(JMUCSession);
	//	Q_ASSERT(room == d->room);
	//	if (d->thread && msg.thread() == *(d->thread)) {
	//		return;
	//	} else if (d->thread) {
	//		delete d->thread;
	//		d->thread = 0;
	//	}
	QString nick = msg.from().resource();
	JMUCUser *user = d->getUser(nick);
	if (priv) {
		if (!user)
			return;
		foreach (Jreen::MessageFilter *filter, d->filters)
			filter->filter(msg);
		if (msg.body().isEmpty())
			return;
		qutim_sdk_0_3::Message coreMsg(msg.body());
		coreMsg.setChatUnit(user);
		coreMsg.setIncoming(true); // msg.from().resource() != d->room->nick());
		ChatSession *chatSession = ChatLayer::get(user, true);
		chatSession->appendMessage(coreMsg);
	} else {
		d->lastMessage = QDateTime::currentDateTime();
		qutim_sdk_0_3::Message coreMsg(msg.body());
		coreMsg.setChatUnit(this);
		coreMsg.setProperty("senderName", nick);
		if (user)
			coreMsg.setProperty("senderId", user->id());
		coreMsg.setIncoming(msg.from().resource() != d->room->nick());
		ChatSession *chatSession = ChatLayer::get(this, true);
		DelayedDelivery::Ptr when = msg.when();
		if (when) {
			coreMsg.setProperty("history", true);
			coreMsg.setTime(when->dateTime());
		} else {
			coreMsg.setTime(d->lastMessage);
		}
		if (!coreMsg.isIncoming() && !when) {
			QHash<QString, quint64>::iterator it = d->messages.find(msg.id());
			if (it != d->messages.end()) {
				qApp->postEvent(chatSession, new qutim_sdk_0_3::MessageReceiptEvent(it.value(), true));
				d->messages.erase(it);
			}
			return;
		}
		if (!msg.subject().isEmpty()) {
			coreMsg.setProperty("topic", true);
			coreMsg.setProperty("subject", msg.subject());
		}
		chatSession->appendMessage(coreMsg);
		if (!msg.subject().isEmpty() && d->topic != msg.subject()) {
			QString oldTopic = d->topic;
			d->topic = msg.subject();
			emit topicChanged(d->topic, oldTopic);
		}
	}
}

void JMUCSession::onServiceMessage(const Jreen::Message &msg)
{
	Q_D(JMUCSession);
	Captcha::Ptr captcha = msg.payload<Captcha>();
	if (captcha && captcha->form()) {
		QString text = tr("Conference \"%1\" requires you to fill the captcha to enter the room")
		               .arg(d->jid.bare());
		delete d->captchaForm.data();
		d->captchaForm = new QWidget;
		QVBoxLayout *layout = new QVBoxLayout(d->captchaForm.data());
		QLabel *label = new QLabel(text, d->captchaForm.data());
		JDataForm *form = new JDataForm(captcha->form(),
		                                      msg.payloads<BitsOfBinary>(),
		                                      AbstractDataForm::Ok | AbstractDataForm::Cancel,
		                                      d->captchaForm.data());
		form->layout()->setMargin(0);
		layout->addWidget(label);
		layout->addWidget(form);
		connect(form, SIGNAL(accepted()), SLOT(onCaptchaFilled()));
		connect(form->widget(), SIGNAL(accepted()), d->captchaForm.data(), SLOT(deleteLater()));
		connect(form->widget(), SIGNAL(rejected()), d->captchaForm.data(), SLOT(deleteLater()));
		Client *client = d->account.data()->client();
		connect(client, SIGNAL(disconnected(Jreen::Client::DisconnectReason)), d->captchaForm.data(), SLOT(deleteLater()));
		d->captchaForm.data()->show();
		return;
	}
	if (!msg.subject().isEmpty())
		return;
	ChatSession *chatSession = ChatLayer::get(this, true);
	qutim_sdk_0_3::Message coreMsg(msg.body());
	coreMsg.setChatUnit(this);
	coreMsg.setProperty("service",true);
	coreMsg.setProperty("silent", true);
	coreMsg.setIncoming(true);
	chatSession->appendMessage(coreMsg);
}

//bool JMUCSession::handleMUCRoomCreation(MUCRoom *room)
//{
//	Q_ASSERT(room == d_func()->room);
//	if(room) {
//		showConfigDialog();
//		return true;
//	}
//	return false;
//}

void JMUCSession::onSubjectChanged(const QString &subject, const QString &nick)
{
	Q_UNUSED(nick);
	Q_D(JMUCSession);
	if (d->topic == subject)
		return;
	qutim_sdk_0_3::Message msg(tr("Subject:") % "\n" % subject);
	msg.setChatUnit(this);
	msg.setTime(QDateTime::currentDateTime());
	msg.setProperty("topic", true);
	msg.setProperty("service", true);
	msg.setIncoming(true);
	if (ChatSession *chatSession = ChatLayer::get(this, false))
		chatSession->appendMessage(msg);
	setConferenceTopic(subject);
}

//void JMUCSession::handleMUCInviteDecline(MUCRoom *room, const JID &invitee, const std::string &reason)
//{
//	Q_ASSERT(room == d_func()->room);
//}

//void JMUCSession::handleMUCError(MUCRoom *room, StanzaError error)
//{
//	Q_D(JMUCSession);
//	Q_ASSERT(room == d->room);
//	bool nnr = false;
//	QString text;
//	switch(error) {
//	case StanzaErrorNotAuthorized:
//		text=tr("Not authorized: Password required.");
//		break;
//	case StanzaErrorForbidden:
//		text=tr("Forbidden: Access denied, user is banned.");
//		break;
//	case StanzaErrorItemNotFound:
//		text=tr("Item not found: The room does not exist.");
//		break;
//	case StanzaErrorNotAllowed:
//		text=tr("Not allowed: Room creation is restricted.");
//		break;
//	case StanzaErrorNotAcceptable :
//		text=tr("Not acceptable: Room nicks are locked down.");
//		break;
//	case StanzaErrorRegistrationRequired:
//		text=tr("Registration required: User is not on the member list.");
//		break;
//	case StanzaErrorConflict:
//		text=tr("Conflict: Desired room nickname is in use or registered by another user.");
//		break;
//	case StanzaErrorServiceUnavailable:
//		text=tr("Service unavailable: Maximum number of users has been reached.");
//		break;
//	default:
//		nnr = true;
//		text=tr("Unknown error: No description.");
//		break;
//	}
//	if (nnr) {

//	} else {
//		QMessageBox::warning(0, tr("Join groupchat on ")%" "%d->account->id(), text);
//		d->account->conferenceManager()->leave(QString::fromStdString(d->jid.full()));
//	}
//	d->isError = true;
//}

//void JMUCSession::handleMUCInfo(MUCRoom *room, int features, const std::string &name, const DataForm *infoForm)
//{
//	Q_ASSERT(room == d_func()->room);
//}

//void JMUCSession::handleMUCItems(MUCRoom *room, const Disco::ItemList &items)
//{
//	Q_ASSERT(room == d_func()->room);
//}

//void JMUCSession::handleMUCConfigList(MUCRoom *room, const MUCListItemList &items, MUCOperation operation)
//{
//	Q_ASSERT(room == d_func()->room);
//}

//void JMUCSession::handleMUCConfigForm(MUCRoom *room, const DataForm &form)
//{
//	Q_ASSERT(room == d_func()->room);
//}

//void JMUCSession::handleMUCConfigResult(MUCRoom *room, bool success, MUCOperation operation)
//{
//	Q_ASSERT(room == d_func()->room);
//}

//void JMUCSession::handleMUCRequest(MUCRoom *room, const DataForm &form)
//{
//	Q_ASSERT(room == d_func()->room);
//}

void JMUCSession::setBookmark(const Jreen::Bookmark::Conference &bookmark)
{
	Q_D(JMUCSession);
	d->bookmark = bookmark;
	QString previous = d->title;
	if (!bookmark.name().isEmpty())
		d->title = bookmark.name();
	else
		d->title = id();
	if (d->title != previous)
		emit titleChanged(d->title, previous);

	emit bookmarkChanged(bookmark);
}

Jreen::Bookmark::Conference JMUCSession::bookmark()
{
	return d_func()->bookmark;
}

QString JMUCSession::title() const
{
	return d_func()->title;
}

void JMUCSession::showConfigDialog()
{
//	//	d_func()->isConfiguring = true;
//	if (d_func()->config)
//		return;
//	d_func()->config = new JConferenceConfig(d_func()->room);
//	//	connect(dialog, SIGNAL(destroyDialog()), SLOT(closeConfigDialog()));
//	d_func()->config->show();
}

void JMUCSession::closeConfigDialog()
{
	//	d_func()->isConfiguring = false;
}

void JMUCSession::joinedChanged()
{
	setJoined(d_func()->room->isJoined());
}

bool JMUCSession::enabledConfiguring()
{
	//TODO add signal configuring changed
	Q_D(JMUCSession);
	if(!isJoined())
		return false;

	MUCRoom::Affiliation affiliation = d->room->affiliation();
	return affiliation == MUCRoom::AffiliationOwner
	        || affiliation == MUCRoom::AffiliationAdmin;
}

void JMUCSession::loadSettings()
{
	d_func()->avatarsAutoLoad = JProtocol::instance()->config("general").value("getavatars", true);
}

void JMUCSession::clearSinceDate()
{
	d_func()->lastMessage = QDateTime();
}

bool JMUCSession::isError()
{
	return d_func()->isError;
}

Jreen::MUCRoom *JMUCSession::room()
{
	return d_func()->room;
}

ChatUnitList JMUCSession::lowerUnits()
{
	ChatUnitList list;
	foreach(JMUCUser *unit, d_func()->users) {
		if (unit->presenceType() != Presence::Unavailable)
			list << unit;
	}
	return list;
}

QString JMUCSession::topic() const
{
	return d_func()->topic;
}

void JMUCSession::setTopic(const QString &topic)
{
	d_func()->room->setSubject(topic);
}

void JMUCSession::setConferenceTopic(const QString &topic)
{
	Q_D(JMUCSession);
	if (d->topic == topic)
		return;
	QString previous = d->topic;
	d->topic = topic;
	emit topicChanged(topic, previous);
}

bool JMUCSession::isAutoJoin()
{
	return d_func()->isAutoRejoin;
}

void JMUCSession::setAutoJoin(bool join)
{
	d_func()->isAutoRejoin = join;
}

void JMUCSession::invite(qutim_sdk_0_3::Contact *contact, const QString &reason)
{
	d_func()->room->invite(contact->id(), reason);
}

void JMUCSession::handleDeath(const QString &name)
{
	d_func()->users.remove(name);
}

void JMUCSession::onError(Jreen::Error::Ptr error)
{
	Q_D(JMUCSession);
	debug() << "error" << error->condition();
	if (error->condition() == Error::Conflict) {
		QString message = QCoreApplication::translate("Jabber", "You are already in conference with another nick");
		NotificationRequest request(Notification::System);
		request.setObject(this);
		request.setText(message);
		request.send();
		QString resource = d->account.data()->client()->jid().resource();
		if (!d->room->nick().endsWith(resource)) {
			QString nick = d->room->nick();
			nick += QLatin1Char('/');
			nick += resource;
			onNickSelected(nick);
			return;
		}

		QInputDialog *dialog = new QInputDialog(QApplication::activeWindow());
		dialog->setWindowTitle(message);
	    dialog->setLabelText(QCoreApplication::translate("Jabber", "Please select another nickname"));
	    dialog->setTextValue(d->room->nick());
		SystemIntegration::show(dialog);
		connect(dialog, SIGNAL(textValueSelected(QString)), SLOT(onNickSelected(QString)));
		connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
	} else if (error->condition() == Error::Forbidden) {
		leave();
	}
}

void JMUCSession::onNickSelected(const QString &nick)
{
	////TODO add regexp
	if(nick.isEmpty()) {
		leave();
	} else {
		Q_D(JMUCSession);
		leave();
		d->room->setNick(nick);
		join();
	}
}

void JMUCSession::onCaptchaFilled()
{
	Q_D(JMUCSession);
	JDataForm *form = qobject_cast<JDataForm*>(sender());
	Client *client = d->account.data()->client();
	Jreen::IQ iq(Jreen::IQ::Set, d->jid.bareJID());
	Captcha::Ptr captcha = Captcha::Ptr::create();
	captcha->setForm(form->getDataForm());
	iq.addPayload(captcha);
	client->send(iq);
}

}

