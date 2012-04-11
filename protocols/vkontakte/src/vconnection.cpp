/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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
#include <QWebInspector>

Q_GLOBAL_STATIC_WITH_ARGS(QString, appId, (QLatin1String("1865463"))) // 1912927"))) //

void VConnectionPrivate::_q_on_error(QNetworkReply::NetworkError)
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(q_func()->sender());
	Q_ASSERT(reply);
	debug() << reply->errorString();
	Status status = account->status();
	status.setChangeReason(Status::ByNetworkError);
	status.setType(qutim_sdk_0_3::Status::Offline);
	account->setStatus(status);
}

void VConnectionPrivate::_q_on_reply_finished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(q_func()->sender());
	Q_ASSERT(reply);
	if (logMode) {
		QByteArray rawData = reply->readAll();
		debug() << "Request:" << reply->url();
		debug() << "Data:" << rawData;
		QVariantMap map = Json::parse(rawData).toMap();
		QVariantMap error = map.value("error").toMap();
		if (!error.isEmpty())
			debug() << error.value("error_msg").toString();
	}
	reply->deleteLater();
}

void VConnectionPrivate::_q_on_webview_destroyed()
{
	if (state != Connected)
		q_func()->setConnectionState(Disconnected);
}


VConnection::VConnection(VAccount* account, QObject* parent):
	QNetworkAccessManager(parent), d_ptr(new VConnectionPrivate(this, account))
{
	Q_D(VConnection);

	d->roster = new VRoster(this);
	d->messages = new VMessages(this);
	d->pollClient = new VLongPollClient(this);

	setProxy(NetworkProxyManager::toNetworkProxy(NetworkProxyManager::settings(account)));
	loadSettings();
}

void VConnection::connectToHost()
{
	Q_D(VConnection);
	if (!d->webView.isNull())
		return;
	d->webView = new QWebView;
	QWebView *webView = d->webView.data();

	webView->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
	QWebInspector *inspector = new QWebInspector;
	inspector->setPage(webView->page());
	connect(webView, SIGNAL(destroyed()), inspector, SLOT(deleteLater()));

	webView->setAttribute(Qt::WA_DeleteOnClose);
#ifdef Q_WS_MAEMO_5
	webView->setParent(qApp->activeWindow());
	webView->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
	webView->page()->setNetworkAccessManager(this);
	webView->setWindowTitle(tr("qutIM - VKontakte authorization"));
	QUrl url("http://vk.com/login.php");
	url.addQueryItem("app", *appId());
#ifdef QUTIM_MOBILE_UI
	url.addQueryItem("layout", "touch");
#else
	url.addQueryItem("layout", "popup");
#endif
	url.addQueryItem("type", "browser");
	url.addQueryItem("settings", QString::number(0x3fff));
	webView->page()->mainFrame()->load(url);

	connect(webView->page(), SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
	connect(webView, SIGNAL(destroyed()), this, SLOT(_q_on_webview_destroyed()));

	setConnectionState(Connecting);
}

void VConnection::onLoadFinished(bool ok)
{
	Q_D(VConnection);
	Q_UNUSED(ok);
	QWebView *webView = d->webView.data();
	QString path = webView->page()->mainFrame()->url().path();
	if (path == QLatin1String("/login.php")) {
		QWebElement email = webView->page()->mainFrame()->findFirstElement("#email");
		email.setAttribute("disabled", "true");
		email.setAttribute("value", d->account->id());
		QString password = d->account->config().group("general")
						   .value("passwd", QString(), Config::Crypted);
		if (!password.isEmpty()) {
			QWebElement pass = webView->page()->mainFrame()->findFirstElement("#pass");
			pass.setAttribute("value", password);
		}

#if 0
		QWebElement connectButton = webView->page()->mainFrame()->findFirstElement("#connect_button");
		connectButton.evaluateJavaScript(QLatin1String("this.click();"));
#endif

		if (!webView->isVisible()) {
			SystemIntegration::show(webView);
		}
		webView->activateWindow();
		webView->raise();
		setConnectionState(Authorization);
	} else {
		webView->deleteLater();
		if (path == QLatin1String("/api/login_success.html")) {
			QByteArray data = webView->page()->mainFrame()->url().fragment().toUtf8();
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
	QUrl url(QLatin1String("http://api.vk.com/api.php"));
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

	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(_q_on_error(QNetworkReply::NetworkError)));
	connect(reply, SIGNAL(finished()), this, SLOT(_q_on_reply_finished()));
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
			 cookieJar()->cookiesForUrl(QUrl("http://vk.com"))) {
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
	cookieJar()->setCookiesFromUrl(cookies, QUrl("http://vk.com"));
	d->logMode = cfg.value("logMode", false);
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

#include "vconnection.moc"
