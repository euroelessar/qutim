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

#ifndef YANDEXNARODAUTHORIZATOR_H
#define YANDEXNARODAUTHORIZATOR_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QNetworkCookieJar>
#include <QPointer>

#define HAS_NO_TOKEN_AUTHORIZATION 1

class QNetworkAccessManager;

class YandexNarodCookieJar : public QNetworkCookieJar
{
	Q_OBJECT
public:
	YandexNarodCookieJar(QNetworkAccessManager *manager);
	
	using QNetworkCookieJar::allCookies;
	using QNetworkCookieJar::setAllCookies;
};

class YandexNarodAuthorizator : public QObject
{
	Q_OBJECT
public:
	enum Result { Success, Failure, Error };

	explicit YandexNarodAuthorizator(QNetworkAccessManager *parent);
	explicit YandexNarodAuthorizator(QWidget *parent);

	QString token() const { return m_token; }
	bool isAuthorized() { return m_stage == Already; }
	void requestAuthorization();
	void requestAuthorization(const QString &login, const QString &password);

	QString resultString(YandexNarodAuthorizator::Result, const QString &error);

public slots:
	void onRequestFinished(QNetworkReply *reply);

signals:
	void result(YandexNarodAuthorizator::Result, const QString &error = QString());
	void needSaveCookies();

private:
	enum Stage { Need, TryingNow, Already };
	Stage m_stage;
	QString m_token;
	QNetworkAccessManager *m_networkManager;
	QPointer<QNetworkReply> m_reply;
};

#endif // YANDEXNARODAUTHORIZATOR_H

