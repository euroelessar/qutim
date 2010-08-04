#include "vwallsession.h"
#include "vwallsession_p.h"
#include "vaccount.h"
#include "vcontact.h"
#include <QNetworkReply>
#include "vconnection.h"
#include <qutim/debug.h>
#include <qutim/json.h>
#include <qutim/messagesession.h>
#include <QTime>
#include <qutim/notificationslayer.h>
#include <qutim/history.h>
#include "vroster.h"

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

	connect(&d->updateTimer,SIGNAL(timeout()),d,SLOT(getHistory()));
	connect(d->owner,SIGNAL(destroyed()),SLOT(deleteLater()));
}

QString VWallSession::id() const
{
	return d_func()->id;
}

QString VWallSession::title() const
{
	Q_D(const VWallSession);
	return d->owner->title();
}


void VWallSession::join()
{
	Q_D(VWallSession);
	d->updateTimer.start();
	d->getHistory();
	ChatSession *session = ChatLayer::get(this,true);
	session->activate();
	connect(session,SIGNAL(destroyed()),SLOT(deleteLater()));
}

void VWallSession::leave()
{
	Q_D(VWallSession);
	d->updateTimer.stop();
	foreach (Contact *contact,d->contacts) {
		if (!contact->isInList())
			contact->deleteLater();
	}
	d->contacts.clear();
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
	return true;
}

VWallSession::~VWallSession()
{
	Q_D(VWallSession);
	leave();

	if (!d->owner->isInList())
		d->owner->deleteLater();
}

VAccount *VWallSessionPrivate::account()
{
	return static_cast<VAccount*>(q_func()->account());
}

void VWallSessionPrivate::getHistory()
{
	QVariantMap data;
	data.insert("owner_id", id);
	data.insert("offset", 0); //test
	data.insert("count", historyCount);
	QNetworkReply *reply = account()->connection()->get("wall.get", data);
	connect(reply,SIGNAL(finished()),SLOT(onGetHistoryFinished()));
}

void VWallSessionPrivate::onGetHistoryFinished()
{
	Q_Q(VWallSession);
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	QByteArray rawData = reply->readAll();

	qDebug() << Q_FUNC_INFO << rawData;

	QVariantMap data = Json::parse(rawData).toMap();
	QVariantList responce = data.value("response").toList();

	ChatSession *session = ChatLayer::get(q,false);
	if (!session || !responce.count())
		return;

	for (int index = responce.count() - 1;index!=0;index--) {
		QVariantMap item = responce.at(index).toMap();
		if (item.isEmpty())
			continue;

		int ts = item.value("date").toInt();
		QString from_id = item.value("from_id").toString();
		VContact *contact = account()->getContact(from_id,true);

		if (timeStamp >= ts)
			continue;

		session->addContact(contact);
		contacts.append(contact);

		timeStamp = ts;

		if (from_id == account()->uid()) //TODO add message delivered notification
			continue;


		Message mess(item.value("text").toString());
		mess.setChatUnit(q);
		mess.setProperty("senderName",contact->title());
		mess.setProperty("silent",true);
		mess.setProperty("senderId",from_id);

		QVariantMap media = item.value("media").toMap();
		if (!media.isEmpty()) {
			mess.setProperty("service",Notifications::System);
			mess.setText(tr("Multimedia messages is unsupported"));
		}

		mess.setTime(QDateTime::fromTime_t(timeStamp));
		mess.setIncoming(true);
		session->appendMessage(mess);
	}
}
