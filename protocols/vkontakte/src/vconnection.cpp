/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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
#include <QApplication>
#include <qutim/debug.h>
#include <qutim/notification.h>
#include <qutim/json.h>
#include <qutim/networkproxy.h>
#include <qutim/dataforms.h>
#include <qutim/systemintegration.h>
#include "vroster.h"
#include "vmessages.h"
#include "vlongpollclient.h"

Q_GLOBAL_STATIC_WITH_ARGS(QString, appId, (QLatin1String("1865463"))) // 1912927"))) //

void VConnectionPrivate::onError(QNetworkReply::NetworkError)
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	//Notifications::send(reply->errorString());
	debug() << reply->errorString();
	Status status = account->status();
	status.setProperty("changeReason", Status::ByNetworkError);
	status.setType(qutim_sdk_0_3::Status::Offline);
	account->setStatus(status);
}

void VConnectionPrivate::onReplyFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	if (logMode) {
		QByteArray rawData = reply->readAll();
		debug() << "Request:" << reply->url();
		debug() << "Data:" << rawData;
		QVariantMap map = Json::parse(rawData).toMap();
		QVariantMap error = map.value("error").toMap();
		if (!error.isEmpty())
			debug() << error.value("error_msg").toString();
			//Notifications::send(error.value("error_msg").toString());
	}
	reply->deleteLater();
}


VConnection::VConnection(VAccount* account, QObject* parent): QNetworkAccessManager(parent), d_ptr(new VConnectionPrivate)
{
	Q_D(VConnection);
	d->q_ptr = this;
	d->account = account;
	d->state = Disconnected;
	d->roster = new VRoster(this,this);
	d->messages = new VMessages(this,this);
	d->logMode = false;
	new VLongPollClient(this);
	setProxy(NetworkProxyManager::toNetworkProxy(NetworkProxyManager::settings(account)));
	loadSettings();
}

void VConnection::connectToHost(const QString& passwd)
{
	Q_D(VConnection);
	Q_UNUSED(passwd)
	if (d->webView)
		return;
	d->webView = new QWebView();
#ifdef Q_WS_MAEMO_5
	d->webView->setParent(qApp->activeWindow());
	d->webView->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
	d->webView->setWindowFlags(d->webView->windowFlags() | Qt::Window);
	d->webView->page()->setNetworkAccessManager(this);
	d->webView->setWindowTitle(tr("qutIM - VKontakte authorization"));
	QUrl url("http://vkontakte.ru/login.php");
	url.addQueryItem("app", *appId());
#ifdef QUTIM_MOBILE_UI
	url.addQueryItem("layout", "touch");
#else
	url.addQueryItem("layout", "popup");
#endif
	url.addQueryItem("type", "browser");
	url.addQueryItem("settings", QString::number(0x3fff));
	d->webView->page()->mainFrame()->load(url);
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
		if (!d->webView->isVisible()) {
			SystemIntegration::show(d->webView.data());
			d->webView->activateWindow();
			d->webView->raise();
		}
		setConnectionState(Authorization);
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
		d->webView->hide();
		d->webView->deleteLater();
		setConnectionState(Disconnected);
	}
}

void VConnection::disconnectFromHost(bool force)
{
	Q_UNUSED(force);
	setConnectionState(Disconnected);
	foreach (QNetworkReply *reply, findChildren<QNetworkReply*>())
		reply->abort();
}

VConnectionState VConnection::connectionState() const
{
	return d_func()->state;
}

void VConnection::setConnectionState(VConnectionState state)
{
	Q_D(VConnection);

	Status origin = d->account->status();
	d->state = state;
	origin.setType(stateToStatus(state));
	d->account->setStatus(origin);
	emit connectionStateChanged(state);
}

VConnection::~VConnection()
{
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
			map.insert(it.key(), value.toStringList().join(QLatin1String(",")));
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
	connect(reply, SIGNAL(finished()), d, SLOT(onReplyFinished()));
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

void VConnection::saveSettings()
{
	QVariantList cookies;
	foreach (const QNetworkCookie &cookie,
			 cookieJar()->cookiesForUrl(QUrl("http://vkontakte.ru"))) {
		cookies << cookie.toRawForm(QNetworkCookie::Full);
	}
	config().setValue("cookies", cookies, Config::Crypted);
	d_func()->roster->saveSettings();
}

void VConnection::loadSettings()
{
	Q_D(VConnection);
	Config cfg = config();
	QList<QNetworkCookie> cookies;
	foreach (const QVariant &var, cfg.value("cookies", QVariantList(), Config::Crypted))
		cookies << QNetworkCookie::parseCookies(var.toByteArray());
	cookieJar()->setCookiesFromUrl(cookies, QUrl("http://vkontakte.ru"));
	d->logMode = cfg.value("logMode",false);
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


