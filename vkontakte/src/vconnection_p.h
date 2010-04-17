#ifndef VCONNECTION_P_H
#define VCONNECTION_P_H
#include <QObject>
#include "vkontakte_global.h"
#include <QTimer>
#include <QNetworkReply>

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
	QByteArray remixPasswd;
	QByteArray sid;
	VConnectionState state;
	VConnection *q_ptr;
	QTimer prolongationTimer;
	VRoster *roster;
	VMessages *messages;
public slots:
	void onAuthRequestFinished();
	void onLogoutRequestFinished();
	void sendProlongation();
	void onError(QNetworkReply::NetworkError error);
};

#endif // VCONNECTION_P_H
