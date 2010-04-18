#include "vroster.h"
#include "vrequest.h"
#include "vaccount.h"
#include "vconnection.h"
#include "vcontact.h"
#include "vroster_p.h"
#include "vaccount_p.h"
#include <QNetworkReply>
#include <qutim/debug.h>
#include <QtScript/QScriptEngine>
#include <qutim/contactlist.h>
#include <QCryptographicHash>
#include <QFile>
#include <QDir>

void VRosterPrivate::onGetFriendsRequestFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	QByteArray data = reply->readAll();

    QScriptEngine engine;
    QScriptValue sc_data = engine.evaluate('(' + data + ')');
    QScriptValue sc_cnt = sc_data.property("friends").property("n");

	if (fetchAvatars)
		avatarsUpdater.stop();

    for (int i=0; i<sc_cnt.toInteger(); i++) {
		QScriptValue sc_item = sc_data.property("friends").property("d").property(i);
		QString id = sc_item.property(0).toString();
		VContact *c = connection->account()->getContact(id,false);
		if (!c) {
			c = connection->account()->getContact(id,true);
			c->setName(sc_item.property(1).toString());
			QSet<QString> tags;
			tags << tr("Friends");
			c->setTags(tags);
			c->setInList(true);
			ContactList::instance()->addContact(c);
		}
		QString current_avatar = c->property("avatarUrl").toString();
		QString new_avatar = sc_item.property(2).toString();
		if (current_avatar != new_avatar) {
			c->setProperty("avatarUrl",new_avatar);
			if (fetchAvatars && !avatarsQueue.contains(c))
				avatarsQueue.append(c);
		}
		c->setStatus(sc_item.property(3).toBoolean());
    }
	if (!avatarsQueue.isEmpty() && !avatarsUpdater.isActive())
		avatarsUpdater.start();
}

void VRosterPrivate::onActivityUpdateRequestFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);

	QByteArray data = reply->readAll();

	QScriptEngine engine;
	QScriptValue request = engine.evaluate('(' + data + ')');
	int count = reply->property("count").toInt();
	QScriptValue items = request.property("d");
	for (int i=0;i!=count;i++) {
		const QScriptValue &item = items.property(i);
		QString id = item.property(1).toString();
		QString text = item.property(5).toString();
		VContact *c = connection->account()->getContact(id,false);
		if (!c)
			continue;
		c->setActivity(text);
	}
}

void VRosterPrivate::onAvatarRequestFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	VContact *contact = reply->property("contact").value<VContact *>();
	Q_ASSERT(contact);
	QDir dir = getAvatarsDir();
	if ( !dir.exists() )
		dir.mkdir(dir.path());

	//FIXME
	QString  name = dir.path() + "/" + QCryptographicHash::hash(reply->url().toString().toUtf8(),QCryptographicHash::Md5).toHex();
	QFile file(name);
	if ( !file.exists() )
	{
		if (!file.open(QIODevice::WriteOnly))
			return;
		file.write(reply->readAll());
		file.close();
	}
	contact->setAvatar(name);
}

void VRosterPrivate::updateAvatar()
{
	Q_Q(VRoster);
	if (avatarsQueue.isEmpty()) {
		avatarsUpdater.stop();
		return;
	}

	VContact *c = avatarsQueue.dequeue();
	q->requestAvatar(c);
}

void VRosterPrivate::updateActivity()
{
	Q_Q(VRoster);
	QList<VContact *> contact_list = connection->account()->d_func()->contactsList;
	if (contact_list.isEmpty())
		return;
	int index =  activityUpdater.property("index").toInt();
	q->requestActivity(contact_list.at(index));
	index++;
	if (index == contact_list.count())
		index = 0;
	activityUpdater.setProperty("index",QVariant(index));
}

void VRosterPrivate::onConnectStateChanged(VConnectionState state)
{
	Q_Q(VRoster);
	switch (state) {
		case Connected: {
			q->getFriendList();
			friendListUpdater.start();
			if (getActivity)
				activityUpdater.start();
			break;
		}
		case Disconnected: {
			friendListUpdater.stop();
			activityUpdater.stop();
			avatarsUpdater.stop();
			foreach (VContact *c,connection->account()->findChildren<VContact *>()) {
				c->setStatus(false);
			}
			break;
		}
		default: {
			break;
		}		
	}
}

VRoster::VRoster(VConnection* connection, QObject* parent) : QObject(parent),d_ptr(new VRosterPrivate)
{
	Q_D(VRoster);
	d->connection = connection;
	d->q_ptr = this;
	loadSettings();
	connect(connection,SIGNAL(connectionStateChanged(VConnectionState)),d,SLOT(onConnectStateChanged(VConnectionState)));
	connect(&d->friendListUpdater,SIGNAL(timeout()),this,SLOT(getFriendList()));
	connect(&d->avatarsUpdater,SIGNAL(timeout()),d,SLOT(updateAvatar()));
	connect(&d->activityUpdater,SIGNAL(timeout()),d,SLOT(updateActivity()));
}

VRoster::~VRoster()
{

}

void VRoster::getProfile()
{
	QUrl url("http://userapi.com/data");
	VRequest request(url);
}

void VRoster::getFriendList(int start, int limit)
{
	Q_D(VRoster);
	QUrl url("http://userapi.com/data");
	url.addQueryItem("friends",QString("%1-%2").arg(start).arg(limit));
	url.addQueryItem("id",d->connection->account()->uid());
	VRequest request(url);
	QNetworkReply *reply = d->connection->get(request);
	connect(reply,SIGNAL(finished()),d,SLOT(onGetFriendsRequestFinished()));
}

void VRoster::requestAvatar(VContact *contact)
{
	Q_D(VRoster);
	QString path (contact->property("avatarUrl").toString());
	if (path == "0") //FIXME find correct method
		return;

	QUrl avatar(path);
	if (!avatar.isValid())
		return;
	VRequest request(avatar);
	QNetworkReply *reply = d->connection->get(request);
	reply->setProperty("contact",qVariantFromValue<VContact *>(contact));
	connect(reply,SIGNAL(finished()),d,SLOT(onAvatarRequestFinished()));
}

void VRoster::requestActivity(VContact *contact)
{
	Q_D(VRoster);
	QUrl url("http://userapi.com/data");
	url.addEncodedQueryItem("act","activity");
	url.addQueryItem("id",contact->id());
	url.addEncodedQueryItem("from","0");
	url.addEncodedQueryItem("to","1");
	VRequest request(url);
	QNetworkReply *reply = d->connection->get(request);
	reply->setProperty("count",1);
	connect(reply,SIGNAL(finished()),d,SLOT(onActivityUpdateRequestFinished()));
}

ConfigGroup VRoster::config()
{
	return d_func()->connection->config("roster");
}

void VRoster::loadSettings()
{
	Q_D(VRoster);
	int interval;
	d->friendListUpdater.setInterval(config().value<int>("friendListUpdateInterval",30000));
	ConfigGroup avatars = config().group("avatars");
	interval = avatars.value<int>("interval",5000);
	d->fetchAvatars = (interval > 0);
	d->avatarsUpdater.setInterval(interval);
	ConfigGroup activity = config().group("activity");
	interval = activity.value<int>("interval",5000);
	d->getActivity = (interval > 0);
	d->activityUpdater.setInterval(interval);
}

