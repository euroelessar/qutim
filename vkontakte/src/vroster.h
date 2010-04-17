#ifndef VROSTER_H
#define VROSTER_H

#include <QObject>
#include "vkontakte_global.h"

namespace qutim_sdk_0_3 {
	class ConfigGroup;
}

class VConnection;

class VRosterPrivate;
class VRoster : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VRoster)
public:
	VRoster(VConnection *connection, QObject *parent = 0);
	virtual ~VRoster();
	ConfigGroup config();
public slots:
	void loadSettings();
	void getProfile();
	void getFriendList(int start = 0, int limit = 10000); //TODO I think that we need a way to get information on parts	
private:
	QScopedPointer<VRosterPrivate> d_ptr;
};

#endif // VROSTER_H
