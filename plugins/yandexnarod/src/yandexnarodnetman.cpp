/****************************************************************************
 *  yandexnarodnetman.cpp
 *
 *  Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>
 *                     2010 by Nigmatullin Ruslan <euroelessar@ya.ru>
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

#include "yandexnarodnetman.h"
#include "requestauthdialog.h"
#include <qutim/configbase.h>
#include <qutim/debug.h>
#include <qutim/libqutim_version.h>

using namespace qutim_sdk_0_3;

YandexNarodNetMan::YandexNarodNetMan(QObject *parent) : QObject(parent)
{
	m_networkManager = new QNetworkAccessManager(this);
	connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(netrpFinished(QNetworkReply*)));

	loadSettings();
	loadCookies();

	auth_flag=0;
	m_trackId = 0;
}

YandexNarodNetMan::~YandexNarodNetMan()
{
//qDebug()<<"yandexnarodNetMan terminated";
}

quint32 YandexNarodNetMan::startAuthorization()
{
	ConfigGroup group = Config().group("yandex");
	QString login = group.value("login", QString());
	QString password = group.value("passwd", QString(), Config::Crypted);
	return startAuthorization(login, password);
}

quint32 YandexNarodNetMan::startAuthorization(const QString &login, const QString &password)
{
	QByteArray post = "login=" + QUrl::toPercentEncoding(login)
					  + "&passwd=" + QUrl::toPercentEncoding(password);

	m_networkRequest.setUrl(QUrl("https://passport.yandex.ru/passport?mode=auth"));
	Track track = { m_trackId, Authorize };
	m_tracks.insert(m_networkManager->post(m_networkRequest, post), track);
	return m_trackId++;
}

void YandexNarodNetMan::startGetFilelist() {
	m_action = GetFileList;
	filesnum=0;
	fileids.clear();
//	loadCookies();
	netmanDo();
}

void YandexNarodNetMan::startDelFiles(QStringList delfileids) {
	m_action = DeleteFile;
	fileids = delfileids;
//	loadCookies();
	netmanDo();
}

void YandexNarodNetMan::startUploadFile(QString filearg) {
	filepath = filearg;
	m_action = UploadFile;
	nstep=1;
//	loadCookies();
	netmanDo();
}

void YandexNarodNetMan::loadSettings() {
	m_networkRequest.setRawHeader("Cache-Control", "no-cache");
	m_networkRequest.setRawHeader("Accept", "*/*");
	QByteArray userAgent = "qutIM/";
	userAgent += versionString();
	userAgent += " (U; YB/4.2.0; MRA/5.5; en)";
	m_networkRequest.setRawHeader("User-Agent", userAgent);
}

void YandexNarodNetMan::loadCookies() {
	Config cookies = Config().group("yandex");
	QNetworkCookieJar *netcookjar = m_networkManager->cookieJar();
	QList<QNetworkCookie> cookieList;
	int count = cookies.beginArray("cookies");
	for (int i = 0;i < count; i++) {
		QNetworkCookie netcook;
		Config cookie = cookies.arrayElement(i);
		netcook.setDomain(cookie.value("domain", QString()));
		QString date = cookie.value("expirationDate", QString());
		if (!date.isEmpty())
			netcook.setExpirationDate(QDateTime::fromString(date, Qt::ISODate));
		netcook.setHttpOnly(cookie.value("httpOnly", false));
		netcook.setSecure(cookie.value("secure", false));
		netcook.setDomain(cookie.value("name", QString()));
		netcook.setDomain(cookie.value("path", QString()));
		netcook.setDomain(cookie.value("value", QString()));
		cookieList.append(netcook);
	}
	netcookjar->setCookiesFromUrl(cookieList, QUrl("http://narod.yandex.ru"));
}

void YandexNarodNetMan::saveCookies() {
	Config group = Config().group("yandexnarod");
	group.remove("cookies");
	group.beginArray("cookies");

	QNetworkCookieJar *netcookjar = m_networkManager->cookieJar();
	int i = 0;
	foreach (QNetworkCookie netcook, netcookjar->cookiesForUrl(QUrl("http://narod.yandex.ru"))) {
		if (netcook.isSessionCookie())
			continue;
		Config cookie = group.arrayElement(i++);
		cookie.setValue("domain", netcook.domain());
		cookie.setValue("expirationDate", netcook.expirationDate().toString(Qt::ISODate));
		cookie.setValue("httpOnly", netcook.isHttpOnly());
		cookie.setValue("secure", netcook.isSecure());
		cookie.setValue("name", QString::fromLatin1(netcook.name()));
		cookie.setValue("path", netcook.path());
		cookie.setValue("value", QString::fromLatin1(netcook.value()));
	}
	group.sync();
}

void YandexNarodNetMan::netmanDo()
{
	if (m_action == Authorize) {
		emit statusText(tr("Authorizing..."));
		QByteArray post = "login=" + narodLogin.toLatin1()
						  + "&passwd=" + QUrl::toPercentEncoding(narodPasswd);
		debug() << narodLogin << narodPasswd;
		debug() << post;
		m_networkRequest.setUrl(QUrl("https://passport.yandex.ru/passport?mode=auth"));
		m_networkManager->post(m_networkRequest, post);
	}
}

void YandexNarodNetMan::netrpFinished( QNetworkReply* reply )
{
	TrackMap::iterator it = m_tracks.find(reply);
	if (it == m_tracks.end())
		return;

	Action action = it->action;
	if (action == Authorize) {
		debug() << reply->rawHeader("Set-Cookie");
		QString page = QString::fromUtf8(reply->readAll());
		debug() << page.size() << page;
		saveCookies();
	}
}

