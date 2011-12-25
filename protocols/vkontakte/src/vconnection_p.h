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

