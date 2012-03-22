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

#ifndef VCONNECTION_P_H
#define VCONNECTION_P_H
#include <QObject>
#include "vkontakte_global.h"
#include "vroster.h"
#include "vmessages.h"
#include "vlongpollclient.h"

#include <QTimer>
#include <QNetworkReply>
#include <QtWebKit/QWebView>
#include <QWeakPointer>


class VMessages;
class VRoster;
class VConnection;
class VAccount;
class VConnectionPrivate
{
	Q_DECLARE_PUBLIC(VConnection)
public:
	VConnectionPrivate(VConnection *q, VAccount *account) :
		q_ptr(q), account(account),
		state(Disconnected),
		roster(0),
		messages(0),
		pollClient(0),
		logMode(false) {}
	VConnection *q_ptr;
	VAccount *account;
	QString secret;
	QString sid;
	QString mid;
	VConnectionState state;
	VRoster *roster;
	VMessages *messages;
	VLongPollClient *pollClient;
	QWeakPointer<QWebView> webView;
	bool logMode;

	void _q_on_error(QNetworkReply::NetworkError error);
	void _q_on_reply_finished();
	void _q_on_webview_destroyed();
};

#endif // VCONNECTION_P_H

