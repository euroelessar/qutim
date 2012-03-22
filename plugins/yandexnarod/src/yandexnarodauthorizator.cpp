/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "yandexnarodauthorizator.h"
#include "requestauthdialog.h"
#include <qutim/configbase.h>
#include <qutim/debug.h>
#include <qutim/json.h>
#include <qutim/libqutim_version.h>
#include <QtCore/QDateTime>
#include <QtNetwork/QNetworkAccessManager>
#include <QtGui/QWidget>

using namespace qutim_sdk_0_3;

#define CLIENT_ID "ecc5ea995f054a6a9acf6a64318bce33"

YandexNarodCookieJar::YandexNarodCookieJar(QNetworkAccessManager *manager)
    : QNetworkCookieJar(manager)
{
	manager->setCookieJar(this);
}

YandexNarodAuthorizator::YandexNarodAuthorizator(QNetworkAccessManager *parent) :
	QObject(parent), m_networkManager(parent)
{
	m_stage = Need;
#if HAS_NO_TOKEN_AUTHORIZATION
	foreach (const QNetworkCookie &cookie,
			 parent->cookieJar()->cookiesForUrl(QUrl("http://narod.yandex.ru"))) {
		if (cookie.name() == "yandex_login" && !cookie.value().isEmpty()) {
			m_stage = Already;
			break;
		}
	}
#else
	Config config;
	config.beginGroup(QLatin1String("yandex"));
	m_token = config.value(QLatin1String("token"), QString(), Config::Crypted);
	
	if (!m_token.isEmpty())
		m_stage = Already;
#endif
	connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(onRequestFinished(QNetworkReply*)));
}

YandexNarodAuthorizator::YandexNarodAuthorizator(QWidget *parent) :
		QObject(parent), m_networkManager(new QNetworkAccessManager(this))
{
	m_stage = Need;
	connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(onRequestFinished(QNetworkReply*)));
}

void YandexNarodAuthorizator::requestAuthorization()
{
	if (m_stage > Need) {
		if (m_stage == Already)
			emit result(Success);
		return;
	}
	ConfigGroup group = Config().group("yandex");
	QString login = group.value("login", QString());
	QString password = group.value("passwd", QString(), Config::Crypted);
	if (login.isEmpty() || password.isEmpty()) {
		YandexNarodRequestAuthDialog dialog;
		dialog.show();
		dialog.setLogin(login);
		if (dialog.exec()) {
			login = dialog.getLogin();
			password = dialog.getPasswd();
			if (dialog.getRemember()) {
				group.setValue("login", login);
				group.setValue("passwd", password, Config::Crypted);
				group.sync();
			}
		} else {
			emit result(Failure, tr("Has no login or password"));
		}
	}
	return requestAuthorization(login, password);
}

QString YandexNarodAuthorizator::resultString(YandexNarodAuthorizator::Result result, const QString &error)
{
	if (result == YandexNarodAuthorizator::Success)
		return tr("Authorization succeed");
	else if (result == YandexNarodAuthorizator::Error)
		return error;
	else
		return tr("Authorization failured");
}

void YandexNarodAuthorizator::requestAuthorization(const QString &login, const QString &password)
{
	if (m_stage > Need) {
		if (m_stage == Already)
			emit result(Success);
		return;
	}
	
#if HAS_NO_TOKEN_AUTHORIZATION
	QByteArray post = "login=" + QUrl::toPercentEncoding(login)
					  + "&passwd=" + QUrl::toPercentEncoding(password)
					  + "&twoweeks=yes";
	QNetworkRequest request(QUrl(QLatin1String("https://passport.yandex.ru/passport?mode=auth")));
	request.setRawHeader("Cache-Control", "no-cache");
	request.setRawHeader("Accept", "*/*");
	QByteArray userAgent = "qutIM/";
	userAgent += versionString();
	userAgent += " (U; YB/4.2.0; MRA/5.5; en)";
	request.setRawHeader("User-Agent", userAgent);
#else
	QByteArray post = "grant_type=password&client_id=" CLIENT_ID
	        "&username=" + QUrl::toPercentEncoding(login)
	        + "&password=" + QUrl::toPercentEncoding(password);
	debug() << post;
	QNetworkRequest request(QUrl(QLatin1String("https://oauth.yandex.ru/token"));
#endif
	request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
	m_reply = m_networkManager->post(request, post);
}

void YandexNarodAuthorizator::onRequestFinished(QNetworkReply *reply)
{
	reply->deleteLater();
	if (reply != m_reply.data())
		return;

#if HAS_NO_TOKEN_AUTHORIZATION
	if (reply->error() != QNetworkReply::NoError) {
		debug() << reply->error() << reply->errorString();
		emit result(Error, reply->errorString());
		return;
	}

	foreach (const QNetworkCookie &cookie,
			 m_networkManager->cookieJar()->cookiesForUrl(QUrl("http://narod.yandex.ru"))) {
		if (cookie.name() == "yandex_login" && !cookie.value().isEmpty()) {
			m_stage = Already;
			emit result(Success);
			emit needSaveCookies();
			return;
		}
	}

	m_stage = Need;
	emit result(Failure);
#else
	QVariantMap data = Json::parse(reply->readAll()).toMap();
	QVariantMap::Iterator error = data.find(QLatin1String("error"));
	if (error != data.end() || reply->error() != QNetworkReply::NoError) {
		QString errorStr = error.value().toString();
		m_stage = Need;
		if (errorStr == QLatin1String("unsupported_grant_type"))
			emit result(Error, tr("Unsupported grant type. Inform developers."));
		else if (errorStr == QLatin1String("invalid_request"))
			emit result(Error, tr("Invalid request. Inform developers."));
		else
			emit result(Error, tr("Invalid login or/and password"));
		return;
	}
	QString accessToken = data.value(QLatin1String("access_token")).toString();
	QDateTime expiresAt;
	QVariantMap::Iterator expiresIn = data.find(QLatin1String("expires_in"));
	if (expiresIn != data.end()) {
		expiresAt = QDateTime::currentDateTime();
		expiresAt.addSecs(expiresIn.value().toInt());
	}
	m_token = accessToken;
	m_stage = Already;
	emit result(Success);
	emit needSaveCookies();
#endif
}

