#ifndef VROSTER_P_H
#define VROSTER_P_H
#include <QObject>
#include "vkontakte_global.h"
#include <QTimer>

class VConnection;
class VRoster;
class VRosterPrivate : QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(VRoster)
public:
	VConnection *connection;
	VRoster *q_ptr;
	int limit;
	int friendListUpdateInterval;
	QTimer friendListUpdater;
public slots:
	void onGetFriendsRequest();
	void onConnectStateChanged(VConnectionState state);
};

#endif // VROSTER_P_H
