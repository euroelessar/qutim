#ifndef VROSTER_P_H
#define VROSTER_P_H
#include <QObject>

class VRoster;
class VAccount;
class VRosterPrivate : QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(VRoster)
public:
	VAccount *account;
	VRoster *q_ptr;
public slots:
	void onGetFriendsRequest();
};

#endif // VROSTER_P_H
