#include "vroster.h"
#include "vrequest.h"
#include "vaccount.h"
#include "vconnection.h"
#include "vcontact.h"
#include "vroster_p.h"
#include <QNetworkReply>
#include <qutim/debug.h>
#include <QtScript/QScriptEngine>
#include <qutim/contactlist.h>

void VRosterPrivate::onGetFriendsRequest()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	QByteArray data = reply->readAll();
	
//     data.replace("\\/","/");
//     data.replace("\\t", " ");

    QScriptEngine engine;
    QScriptValue sc_data = engine.evaluate('(' + data + ')');
    QScriptValue sc_cnt = sc_data.property("friends").property("n");

    for (int i=0; i<sc_cnt.toInteger(); i++) {
		QScriptValue sc_item = sc_data.property("friends").property("d").property(i);
		QString id = sc_item.property(0).toString();
		VContact *c = connection->account()->getContact(id,false);
		if (c) {
			c->setStatus(sc_item.property(3).toBoolean());
			c->setProperty("avatarUrl",sc_item.property(2).toString());
		}
		else {
			VContact *c = connection->account()->getContact(id,true);
			c->setName(sc_item.property(1).toString());
			QSet<QString> tags;
			tags << tr("Friends");
			c->setTags(tags);
			c->setProperty("avatarUrl",sc_item.property(2).toString());
			c->setStatus(sc_item.property(3).toBoolean());
			c->setInList(true);
			ContactList::instance()->addContact(c);
		}
    }

}

void VRosterPrivate::onConnectStateChanged(VConnectionState state)
{
	Q_Q(VRoster);
	switch (state) {
		case Connected: {
			q->getFriendList();
			friendListUpdater.start();
			break;
		}
		case Disconnected: {
			friendListUpdater.stop();
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
	connect(reply,SIGNAL(finished()),d,SLOT(onGetFriendsRequest()));
}

ConfigGroup VRoster::config()
{
	return d_func()->connection->config("roster");
}

void VRoster::loadSettings()
{
	Q_D(VRoster);
	d->friendListUpdater.setInterval(config().value<int>("friendListUpdateInterval",30000));
}

