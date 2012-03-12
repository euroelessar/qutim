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
#include "vwallsession.h"
#include "vwallsession_p.h"
#include "../../src/vaccount.h"
#include "../../src/vcontact.h"
#include <QNetworkReply>
#include "../../src/vconnection.h"
#include <qutim/debug.h>
#include <qutim/json.h>
#include <qutim/chatsession.h>
#include <QTime>
#include <qutim/notification.h>
#include <qutim/history.h>
#include "../../src/vroster.h"

VWallSession::VWallSession(const QString& id, VAccount* account): Conference(account), d_ptr(new VWallSessionPrivate)
{
	Q_D(VWallSession);
	d->q_ptr = this;
	d->id = id;
	d->me = account->getContact(account->uid(),true);
	d->me->setContactName(account->name());
	d->owner = d->account()->getContact(d->id,true);
	d->historyCount = 10;

	MessageList list = History().read(this,1);
	if (list.count())
		d->timeStamp = list.first().time().toTime_t();
	d->updateTimer.setInterval(5000);

	connect(&d->updateTimer, SIGNAL(timeout()), d, SLOT(getHistory()));
	connect(d->owner, SIGNAL(destroyed()), SLOT(deleteLater()));
}

QString VWallSession::id() const
{
	return d_func()->id;
}

QString VWallSession::title() const
{
	Q_D(const VWallSession);
	return tr("%1 (Wall)").arg(d->owner->title());
}


void VWallSession::doJoin()
{
	Q_D(VWallSession);
	d->updateTimer.start();
	d->getHistory();
	setJoined(true);

	//TODO move to qutim::Conference
	ChatSession *session = ChatLayer::get(this, true);
	session->activate();
	connect(session, SIGNAL(destroyed()), SLOT(deleteLater()));
}

void VWallSession::doLeave()
{
	Q_D(VWallSession);
	d->updateTimer.stop();
	setJoined(false);

	//TODO move this method to qutim::conference
	ChatSession *session = ChatLayer::get(this, false);
	foreach (Contact *contact, d->participants) {
		if (session)
			session->removeContact(contact);
		if (contact != me()
				&& contact != d->owner)
			contact->deleteLater();
	}
}

qutim_sdk_0_3::Buddy* VWallSession::me() const
{
	Q_D(const VWallSession);
	return d->me;
}

bool VWallSession::sendMessage(const qutim_sdk_0_3::Message& message)
{
	Q_D(VWallSession);
	QVariantMap data;
	data.insert("owner_id", d->id);
	data.insert("message", message.text());
	QNetworkReply *reply = d->account()->connection()->get("wall.post", data);
	Q_UNUSED(reply);
	return true;
}

VWallSession::~VWallSession()
{
	leave();
}

VAccount *VWallSessionPrivate::account()
{
	return static_cast<VAccount*>(q_func()->account());
}

void VWallSessionPrivate::getHistory()
{
	QVariantMap data;

	QString query("\
				  var query = ({\"owner_id\":%1,\"offset\":0,\"count\":%2}); \
							   var messages = API.wall.get(query); \
							  query = ({\"uids\":messages@.from_id,\"fields\":\"first_name,last_name,nickname,photo_rec\"}); \
									   var profiles = API.getProfiles(query); \
									  return {\"messages\": messages, \"profiles\": profiles};"
									  );
	query = query.arg(id).arg(historyCount);
	data.insert("code",query);
	QNetworkReply *reply = account()->connection()->get("execute", data);
	connect(reply,SIGNAL(finished()),SLOT(onGetHistoryFinished()));
}

void VWallSessionPrivate::onGetHistoryFinished()
{
	Q_Q(VWallSession);
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	QByteArray rawData = reply->readAll();

	debug() << Q_FUNC_INFO << rawData;

	QVariantMap data = Json::parse(rawData).toMap();
	QVariantMap responce = data.value("response").toMap();
	QVariantList messages = responce.value("messages").toList();
	QVariantList profiles = responce.value("profiles").toList();

	ChatSession *session = ChatLayer::get(q, false);
	if (!session || !messages.count())
		return;

	for (int index = messages.count() - 1;index!=0;index--) {
		QVariantMap msg_item = messages.at(index).toMap();

		if (msg_item.isEmpty())
			continue;

		QVariantMap profile;
		if (index <= profiles.count())
			profile = profiles.at(index-1).toMap();

		int ts = msg_item.value("date").toInt();
		QString from_id = msg_item.value("from_id").toString();
		QString name = from_id;
		if (!profile.isEmpty()) {
			VContact *contact = account()->connection()->roster()->getContact(profile, true);
			contact->setOnline(msg_item.value("online").toBool());
			name = contact->title();
			session->addContact(contact);

			participants.append(contact);
			connect(contact, SIGNAL(destroyed(QObject*)), this, SLOT(participantDestroyed(QObject*)));
		}

		if (timeStamp >= ts)
			continue;

		timeStamp = ts;

		if (from_id == account()->uid()) //TODO add message delivered notification
			continue;


		Message mess(unescape(msg_item.value("text").toString()));
		mess.setChatUnit(q);
		mess.setProperty("senderName",name);
		mess.setProperty("silent",true);
		mess.setProperty("senderId",from_id);

		QVariantMap media = msg_item.value("media").toMap();
		if (!media.isEmpty()) {
			processMultimediaMessage(mess,media);
		}

		mess.setTime(QDateTime::fromTime_t(timeStamp));
		mess.setIncoming(true);
		session->appendMessage(mess);
	}
}

void VWallSessionPrivate::participantDestroyed(QObject *obj)
{
	participants.removeAll(reinterpret_cast<VContact*>(obj));
}

void VWallSessionPrivate::processMultimediaMessage(Message &mess, const QVariantMap &data)
{
	QString orig_type = data.value("type").toString();
	QString type = tr("Unknown");
	if (orig_type == QLatin1String("audio"))
		type = tr("Audio");
	else if (orig_type == QLatin1String("video"))
		type = tr("Video");
	else if (orig_type == QLatin1String("app"))
		type = tr("Application");
	else if (orig_type == QLatin1String("graffiti"))
		type = tr("Graffiti");
	else if (orig_type == QLatin1String("photo"))
		type = tr("Photo");
	else if (orig_type == QLatin1String("posted_photo"))
		type = tr("Posted photo");

	QString html = tr("Multimedia message: %1").arg(orig_type) + QLatin1String("\n") + mess.text();
	QString thumb_src = data.value("thumb_src").toString();
	if (thumb_src.isEmpty()) {
		mess.setProperty("title", mess.property("senderName"));
		mess.setProperty("service", Notification::System);
	}

	html += thumb_src;
	mess.setText(html);
}

