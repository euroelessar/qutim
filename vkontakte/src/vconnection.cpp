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
	reply->deleteLater();
}

void VConnectionPrivate::onLogoutRequestFinished()
{
	Q_Q(VConnection);
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	QByteArray location = reply->rawHeader("Location");
	int index = location.indexOf("sid=");
	QString sid = "-1";
	if (index != -1)
		sid = location.mid(index + 4);

	if (sid == "-1") {
		//TODO
	}

	q->setConnectionState(Disconnected);
	sid = QString();
	remixPasswd = QString();
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
    QUrl url("http://login.userapi.com/auth");
	url.addEncodedQueryItem("login","force");
	url.addEncodedQueryItem("site","2");
	url.addQueryItem("email",d->account->email());
	url.addQueryItem("pass",passwd);
	VRequest auth_request(url);
	QNetworkReply* reply = get(auth_request);
	connect(reply,SIGNAL(finished()),d,SLOT(onAuthRequestFinished()));
	setConnectionState(Connecting);
}

void VConnection::disconnectFromHost(bool force)
{
	Q_D(VConnection);
	if (force) {
		setConnectionState(Disconnected);
		d->sid = QString();
		d->remixPasswd = QString();
		return;
	}
	QUrl url("http://login.userapi.com/auth");
	url.addEncodedQueryItem("login","force");
	url.addEncodedQueryItem("site","2");
	url.addQueryItem("sid",d->sid);
	VRequest logout_request(url);
	QNetworkReply* reply = get(logout_request);
	connect(reply,SIGNAL(finished()),d,SLOT(onLogoutRequestFinished()));
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

QNetworkReply* VConnection::get(VRequest& request)
{
	Q_D(VConnection);
	if (!d->sid.isEmpty()) {
		QUrl url = request.url();
		url.addQueryItem("sid",d->sid);
		request.setUrl(url);
	}
	QNetworkAccessManager::get(request);
}

