#ifndef VROSTER_H
#define VROSTER_H

#include <QObject>
#include "vkontakte_global.h"

class VAccount;
class VConnection;

class VRosterPrivate;
class VRoster : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VRoster)
public:
	VRoster(VAccount *account, QObject *parent = 0);
	void getProfile();
	void getFriendList(int limit = 2000); //TODO I think that we need a way to get information on parts
	virtual ~VRoster();
private:
	QScopedPointer<VRosterPrivate> d_ptr;
};

#endif // VROSTER_H
