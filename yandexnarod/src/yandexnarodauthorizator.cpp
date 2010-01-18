/****************************************************************************
 *  yandexnarodauthorizator.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@ya.ru>
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

#include "yandexnarodauthorizator.h"
#include "requestauthdialog.h"
#include <qutim/configbase.h>
#include <qutim/debug.h>
#include <QtCore/QDateTime>
#include <QtNetwork/QNetworkCookieJar>
#include <QtGui/QWidget>

using namespace qutim_sdk_0_3;

YandexNarodAuthorizator::YandexNarodAuthorizator(QNetworkAccessManager *parent) :
	QObject(parent), m_networkManager(parent)
{
	m_stage = Need;
	foreach (const QNetworkCookie &cookie,
			 parent->cookieJar()->cookiesForUrl(QUrl("http://narod.yandex.ru"))) {
		if (cookie.name() == "yandex_login" && !cookie.value().isEmpty()) {
			m_stage = Already;
			break;
		}
	}

	connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(onRequestFinished(QNetworkReply*)));
}

YandexNarodAuthorizator::YandexNarodAuthorizator(QWidget *parent) :
		QObject(parent), m_networkManager(new QNetworkAccessManager(this))
{
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
	QByteArray post = "login=" + QUrl::toPercentEncoding(login)
					  + "&passwd=" + QUrl::toPercentEncoding(password)
					  + "&twoweeks=yes";
	QNetworkRequest request(QUrl("https://passport.yandex.ru/passport?mode=auth"));
	request.setRawHeader("Cache-Control", "no-cache");
	request.setRawHeader("Accept", "*/*");
	QByteArray userAgent = "qutIM/";
	userAgent += qutimVersionStr();
	userAgent += " (U; YB/4.2.0; MRA/5.5; en)";
	request.setRawHeader("User-Agent", userAgent);
	m_reply = m_networkManager->post(request, post);
}

void YandexNarodAuthorizator::onRequestFinished(QNetworkReply *reply)
{
	debug() << Q_FUNC_INFO << m_reply.data() << reply;
	if (reply != m_reply)
		return;

	reply->deleteLater();
	if (reply->error() != QNetworkReply::NoError) {
		debug() << reply->error() << reply->errorString();
		emit result(Error, reply->errorString());
		return;
	}

	foreach (const QNetworkCookie &cookie,
			 m_networkManager->cookieJar()->cookiesForUrl(QUrl("http://narod.yandex.ru"))) {
		debug() << cookie.name() << cookie.value() << cookie.expirationDate();
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

	QString page = QString::fromUtf8(reply->readAll());
	debug() << page;

	m_stage = Need;
	emit result(Failure);
}
