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
	QTimer prolongationTimer;
	VRoster *roster;
	VMessages *messages;
	QPointer<QWebView> webView;
public slots:
	void onAuthRequestFinished();
	void onLogoutRequestFinished();
	void sendProlongation();
	void onError(QNetworkReply::NetworkError error);
};

#endif // VCONNECTION_P_H
