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
	
    debug()<<"processProfileData"<<data;
	
    data.replace("\\/","/");
    data.replace("\\t", " ");

    QScriptEngine engine;
    QScriptValue sc_data = engine.evaluate('(' + data + ')');
    QScriptValue sc_cnt = sc_data.property("friends").property("n");

    for (int i=0; i<sc_cnt.toInteger(); i++) {
		QScriptValue sc_item = sc_data.property("friends").property("d").property(i);
		VContact *c = new VContact(sc_item.property(0).toString(),account);
		c->setName(sc_item.property(1).toString());
		QSet<QString> tags;
		tags << tr("Friends");
		c->setTags(tags);
		//f_buddy.m_avatar_url = sc_item.property(2).toString();
		c->setStatus(sc_item.property(3).toBoolean());
		c->setInList(true);
		debug() << c->id() << c->name() << sc_item.property(0).toString();
		ContactList::instance()->addContact(c);
    }

}

VRoster::VRoster(VAccount* account,QObject *parent) : QObject(parent),d_ptr(new VRosterPrivate)
{
	Q_D(VRoster);
	d->account = account;
	d->q_ptr = this;
}

VRoster::~VRoster()
{

}

void VRoster::getProfile()
{
	QUrl url("http://userapi.com/data");
	VRequest request(url);
}

void VRoster::getFriendList(int limit)
{
	Q_D(VRoster);
	QUrl url("http://userapi.com/data");
	url.addQueryItem("friends",QString("0-%1").arg(limit));
	url.addQueryItem("id",d->account->uid());
	//url.addEncodedQueryItem("back","friends");
	VRequest request(url);
	QNetworkReply *reply = d->account->connection()->get(request);
	debug() << request.url();
	connect(reply,SIGNAL(finished()),d,SLOT(onGetFriendsRequest()));
}
