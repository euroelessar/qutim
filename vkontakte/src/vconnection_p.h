#ifndef VCONNECTION_P_H
#define VCONNECTION_P_H
#include <QObject>
#include "vkontakte_global.h"

class VConnection;
class VAccount;
class VConnectionPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(VConnection)
public:
	VAccount *account;
	QString remixPasswd;
	QString sid;
	VConnectionState state;
	VConnection *q_ptr;
	int prolongationTimerId;
public slots:
	void onAuthRequestFinished();
	void onLogoutRequestFinished();
	void onConnectionStateChanged(int state);
};

#endif // VCONNECTION_P_H
