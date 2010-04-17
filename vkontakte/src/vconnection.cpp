#include "vconnection.h"
#include "vaccount.h"
#include "vconnection_p.h"
#include "vrequest.h"
#include <QNetworkReply>
#include <QNetworkCookie>
#include <qutim/debug.h>
#include <qutim/notificationslayer.h>
#include "vroster.h"
#include "vmessages.h"

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
		Notifications::sendNotification(tr("Error! (TODO)"));
		q->setConnectionState(Disconnected);
	}
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
	remixPasswd.clear();
}

void VConnectionPrivate::onError(QNetworkReply::NetworkError)
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	Notifications::sendNotification(reply->errorString());
}

void VConnectionPrivate::sendProlongation()
{
	Q_Q(VConnection);
	QUrl url ("http://login.userapi.com/auth");
	url.addEncodedQueryItem("login","auto");
	url.addEncodedQueryItem("site","2");
	VRequest request(url);
	request.setRawHeader("remixpassword",remixPasswd);
	q->get(request);
}

VConnection::VConnection(VAccount* account, QObject* parent): QNetworkAccessManager(parent), d_ptr(new VConnectionPrivate)
{
	Q_D(VConnection);
	d->q_ptr = this;
	d->account = account;
	d->state = Disconnected;
	d->roster = new VRoster(this,this);
	d->messages = new VMessages(this,this);
	loadSettings();
	connect(&d->prolongationTimer,SIGNAL(timeout()),d,SLOT(sendProlongation()));
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
		d->sid.clear();
		d->remixPasswd.clear();
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
	Q_D(VConnection);
	if (state == Connected)
		d->prolongationTimer.start();
	else
		d->prolongationTimer.stop();
	d->state = state;
	d->account->setStatus(stateToStatus(state));
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
	else
		debug() << "request" << request.url();
	QNetworkReply *reply = QNetworkAccessManager::get(request);
	connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),d,SLOT(onError(QNetworkReply::NetworkError)));
	connect(reply,SIGNAL(finished()),reply,SLOT(deleteLater()));
	return reply;
}

ConfigGroup VConnection::config()
{
	return d_func()->account->config("connection");
}
ConfigGroup VConnection::config(const QString& name)
{
	return config().group(name);
}


void VConnection::loadSettings()
{
	Q_D(VConnection);
	d->prolongationTimer.setInterval(config().value<int>("prolongationInterval",90000));
}

VAccount* VConnection::account() const
{
	return d_func()->account;
}

VMessages* VConnection::messages() const
{
	return d_func()->messages;
}

VRoster* VConnection::roster() const
{
	return d_func()->roster;
}


