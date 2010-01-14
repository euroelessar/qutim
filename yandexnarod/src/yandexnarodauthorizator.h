/****************************************************************************
 *  yandexnarodauthorizator.р
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

#ifndef YANDEXNARODAUTHORIZATOR_H
#define YANDEXNARODAUTHORIZATOR_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QPointer>

class YandexNarodAuthorizator : public QObject
{
	Q_OBJECT
public:
	enum Result { Success, Failure, Error };

	explicit YandexNarodAuthorizator(QNetworkAccessManager *parent);
	explicit YandexNarodAuthorizator(QWidget *parent);

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
	QNetworkAccessManager *m_networkManager;
	QPointer<QNetworkReply> m_reply;
};

#endif // YANDEXNARODAUTHORIZATOR_H
