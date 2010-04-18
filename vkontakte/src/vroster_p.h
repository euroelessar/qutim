#ifndef VROSTER_P_H
#define VROSTER_P_H
#include <QObject>
#include "vkontakte_global.h"
#include <QTimer>
#include <QQueue>

class VConnection;
class VRoster;
class VContact;
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
	QTimer avatarsUpdater;
	QTimer activityUpdater;
	QQueue<VContact *> avatarsQueue;
	bool fetchAvatars;
	bool getActivity;
public slots:
	void onGetFriendsRequestFinished();
	void onConnectStateChanged(VConnectionState state);
	void onAvatarRequestFinished();
	void onActivityUpdateRequestFinished();
	void updateAvatar();
	void updateActivity();
};

#endif // VROSTER_P_H
