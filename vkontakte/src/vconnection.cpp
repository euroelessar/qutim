#include "vconnection.h"
#include "vaccount.h"
#include "vconnection_p.h"
#include "vrequest.h"
#include <QNetworkReply>
#include <QNetworkCookie>
#include <qutim/debug.h>

void VConnectionPrivate::onAuthRequestFinished()
{
	Q_Q(VConnection);
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	QList<QNetworkCookie> cookie_list = reply->manager()->cookieJar()->cookiesForUrl(QUrl("http://login.userapi.com"));
	QByteArray location = reply->rawHeader("Location");
	int index = location.indexOf("sid=");
	if (index != -1) {
	    foreach(QNetworkCookie cookie, cookie_list)
	    {
		if ( cookie.name() == "remixpassword" && cookie.value() != "deleted" )
		{
		    remixPasswd = cookie.value();
		    sid = location.mid(index + 4);
		}
		if ( cookie.name() == "remixmid" )
		    account->setUid(cookie.value());
	    }
		q->setConnectionState(Connected);
	}
	else {
		q->setConnectionState(Disconnected);
	}
	debug() << remixPasswd << sid;
}

void VConnectionPrivate::onConnectionStateChanged(int state)
{
	Status status(stateToStatus(static_cast<VConnectionState>(state)));
	account->setStatus(status);
}

VConnection::VConnection(VAccount* account, QObject* parent): QNetworkAccessManager(parent), d_ptr(new VConnectionPrivate)
{
	Q_D(VConnection);
	d->q_ptr = this;
	d->account = account;
	d->state = Disconnected;
	connect(this,SIGNAL(connectionStateChanged(int)),d,SLOT(onConnectionStateChanged(int)));
}

void VConnection::connectToHost(const QString& passwd)
{
	Q_D(VConnection);
    QString sQuery = QString("login=force&site=2&email=%1&pass=").arg(d->account->id()) + passwd;
    QUrl url("http://login.userapi.com/auth?");
    url.setEncodedQuery(sQuery.toUtf8());
	VRequest auth_request(url);
	debug() <<  auth_request.rawHeaderList() << auth_request.url();
	QNetworkReply* reply = get(auth_request);
	connect(reply,SIGNAL(finished()),d,SLOT(onAuthRequestFinished()));
	setConnectionState(Connecting);
}

void VConnection::disconnectFromHost()
{
	setConnectionState(Disconnected);
}

VConnectionState VConnection::connectionState() const
{
	return d_func()->state;
}

void VConnection::setConnectionState(VConnectionState state)
{
	d_func()->state = state;
	emit connectionStateChanged(state);
}

VConnection::~VConnection()
{

}
