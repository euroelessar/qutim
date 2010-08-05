/****************************************************************************
 *  vconnection_p.h
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

#ifndef VCONNECTION_P_H
#define VCONNECTION_P_H
#include <QObject>
#include "vkontakte_global.h"
#include <QTimer>
#include <QNetworkReply>
#include <QtWebKit/QWebView>
#include <QPointer>

class VMessages;
class VRoster;
class VConnection;
class VAccount;
class VConnectionPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(VConnection)
public:
	VAccount *account;
	QString secret;
	QString sid;
	QString mid;
	VConnectionState state;
	VConnection *q_ptr;
	VRoster *roster;
	VMessages *messages;
	QPointer<QWebView> webView;
	bool logMode;
public slots:
	void onError(QNetworkReply::NetworkError error);
	void onReplyFinished();
};

#endif // VCONNECTION_P_H
