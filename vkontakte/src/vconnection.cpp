/****************************************************************************
 *  vconnection.cpp
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#include "vconnection.h"
#include "vaccount.h"
#include "vconnection_p.h"
#include "vrequest.h"
#include <QNetworkReply>
#include <QNetworkCookie>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebElement>
#include <QVariantMap>
#include <QCryptographicHash>
#include <qutim/debug.h>
#include <qutim/notificationslayer.h>
#include <qutim/json.h>
#include "vroster.h"
#include "vmessages.h"

Q_GLOBAL_STATIC_WITH_ARGS(QString, appId, (QLatin1String("1912927"))) // 1865463")))

void VConnectionPrivate::onAuthRequestFinished()
{
//	Q_Q(VConnection);
//	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
//	Q_ASSERT(reply);
//	QList<QNetworkCookie> cookie_list = reply->manager()->cookieJar()->cookiesForUrl(QUrl("http://login.userapi.com"));
//	QByteArray location = reply->rawHeader("Location");
//	int index = location.indexOf("sid=");
//	if (index != -1) {
//	    foreach(QNetworkCookie cookie, cookie_list)
//	    {
//		if ( cookie.name() == "remixpassword" && cookie.value() != "deleted" )
//		{
//		    remixPasswd = cookie.value();
//		    sid = location.mid(index + 4);
//		}
//		if ( cookie.name() == "remixmid" )
//		    account->setUid(cookie.value());
//	    }
//		q->setConnectionState(Connected);
//	}
//	else {
//		Notifications::sendNotification(tr("Error! (TODO)"));
//		q->setConnectionState(Disconnected);
//	}
}

void VConnectionPrivate::onLogoutRequestFinished()
{
//	Q_Q(VConnection);
//	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
//	Q_ASSERT(reply);
//	QByteArray location = reply->rawHeader("Location");
//	int index = location.indexOf("sid=");
//	QString sid = "-1";
//	if (index != -1)
//		sid = location.mid(index + 4);
//
//	if (sid == "-1") {
//		//TODO
//	}
//
//	q->setConnectionState(Disconnected);
//	sid = QString();
//	remixPasswd.clear();
}

void VConnectionPrivate::onError(QNetworkReply::NetworkError)
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	Notifications::sendNotification(reply->errorString());
}

void VConnectionPrivate::sendProlongation()
{
//	Q_Q(VConnection);
//	QUrl url ("http://login.userapi.com/auth");
//	url.addEncodedQueryItem("login","auto");
//	url.addEncodedQueryItem("site","2");
//	VRequest request(url);
//	request.setRawHeader("remixpassword",remixPasswd);
//	q->get(request);
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
	Q_UNUSED(passwd)
	if (d->webView)
		return;
	d->webView = new QWebView;
	d->webView->page()->setNetworkAccessManager(this);
	QUrl url("http://vkontakte.ru/login.php");
	url.addQueryItem("app", *appId());
#ifdef QUTIM_MOBILE_UI
	url.addQueryItem("layout", "touch");
#else
	url.addQueryItem("layout", "popup");
#endif
	url.addQueryItem("type", "browser");
	url.addQueryItem("settings", QString::number(0xffff));
	d->webView->page()->mainFrame()->load(url);
#ifdef QUTIM_MOBILE_UI
	d->webView->showMaximized();
#else
	d->webView->show();
#endif
	connect(d->webView->page(), SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
	setConnectionState(Connecting);
}

void VConnection::onLoadFinished(bool ok)
{
	Q_D(VConnection);
	Q_UNUSED(ok);
	QString path = d->webView->page()->mainFrame()->url().path();
	if (path == QLatin1String("/login.php")) {
		QWebElement email = d->webView->page()->mainFrame()->findFirstElement("#email");
		email.setAttribute("disabled", "true");
		email.setAttribute("value", d->account->id());
		QString password = d->account->config().group("general")
						   .value("passwd", QString(), Config::Crypted);
		if (!password.isEmpty()) {
			QWebElement pass = d->webView->page()->mainFrame()->findFirstElement("#pass");
			pass.setAttribute("value", password);
		}
	} else if (path == QLatin1String("/api/login_success.html")) {
		d->webView->hide();
		d->webView->deleteLater();
		QByteArray data = d->webView->page()->mainFrame()->url().fragment().toUtf8();
		data = data.mid(data.indexOf('=') + 1);
		QVariantMap map = Json::parse(data).toMap();
		d->sid = map.value("sid").toString();
		d->mid = map.value("mid").toString();
		d->account->setUid(d->mid);
		d->secret = map.value("secret").toString();
		setConnectionState(Connected);
	} else {
		setConnectionState(Disconnected);
	}
}

void VConnection::disconnectFromHost(bool force)
{
	Q_D(VConnection);
	Q_UNUSED(force);
//	if (force) {
//		setConnectionState(Disconnected);
//		d->sid.clear();
//		d->remixPasswd.clear();
//		return;
//	}
//	QUrl url("http://login.userapi.com/auth");
//	url.addEncodedQueryItem("login","force");
//	url.addEncodedQueryItem("site","2");
//	url.addQueryItem("sid",d->sid);
//	VRequest logout_request(url);
//	QNetworkReply* reply = get(logout_request);
//	connect(reply,SIGNAL(finished()),d,SLOT(onLogoutRequestFinished()));
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
	Q_UNUSED(request);
//	Q_D(VConnection);
//	if (!d->sid.isEmpty()) {
//		QUrl url = request.url();
//		url.addQueryItem("sid",d->sid);
//		request.setUrl(url);
//		debug() << "request" << request.url();
//	}
//	QNetworkReply *reply = QNetworkAccessManager::get(request);
//	connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),d,SLOT(onError(QNetworkReply::NetworkError)));
//	connect(reply,SIGNAL(finished()),reply,SLOT(deleteLater()));
//	return reply;
	return 0;
}

QNetworkReply *VConnection::get(const QString &method, const QVariantMap &args)
{
	Q_D(VConnection);
	QUrl url(QLatin1String("http://api.vkontakte.ru/api.php"));
	QMap<QString, QString> map;
	map.insert("api_id", *appId());
	map.insert("method", method);
	map.insert("v", "3.0");
	map.insert("format", "json");
	QVariantMap::const_iterator it = args.constBegin();
	for (; it != args.constEnd(); it++) {
		QVariant value = it.value();
		if (value.canConvert<QStringList>())
			map.insert(it.key(), value.toStringList().join(","));
		else
			map.insert(it.key(), value.toString());
	}
	QCryptographicHash hash(QCryptographicHash::Md5);
	hash.addData(d->mid.toUtf8());
	QMap<QString, QString>::const_iterator jt = map.constBegin();
	for (; jt != map.constEnd(); jt++) {
		url.addQueryItem(jt.key(), jt.value());
		hash.addData(jt.key().toUtf8());
		hash.addData("=", 1);
		hash.addData(jt.value().toUtf8());
	}
	hash.addData(d->secret.toUtf8());
	url.addQueryItem("sig", hash.result().toHex());
	url.addQueryItem("sid", d->sid);
	QNetworkRequest request(url);
	QNetworkReply *reply = QNetworkAccessManager::get(request);
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), d, SLOT(onError(QNetworkReply::NetworkError)));
	connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
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


