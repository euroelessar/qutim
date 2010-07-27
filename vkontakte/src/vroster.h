#ifndef VROSTER_H
#define VROSTER_H

#include <QObject>
#include "vkontakte_global.h"

namespace qutim_sdk_0_3 {
	class Config;
}

class VConnection;
class VContact;
class VRosterPrivate;
class VRoster : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VRoster)
public:
	VRoster(VConnection *connection, QObject *parent = 0);
	virtual ~VRoster();
	Config config();
public slots:
	void loadSettings();
	void getProfile();
	void getTagList();
	void getFriendList(int start = 0, int limit = 10000); //TODO I think that we need a way to get information on parts	
	void requestAvatar(QObject *contact);
	void requestActivity(VContact *contact);
private:
	QScopedPointer<VRosterPrivate> d_ptr;
};

#endif // VROSTER_H
