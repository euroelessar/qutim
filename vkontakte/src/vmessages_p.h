#ifndef VMESSAGES_P_H
#define VMESSAGES_P_H

#include <QObject>
#include "vkontakte_global.h"
#include <QTimer>

class VMessages;
class VConnection;

class VMessagesPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(VMessages)
public:
	VConnection *connection;
	VMessages *q_ptr;
	int unreadMessageCount;
	QTimer historyTimer;
	void markAsRead(const QStringList &messages);
public slots:
	void onConnectStateChanged(VConnectionState state);
	void onHistoryRecieved(); //TODO move to vconnection
	void onMessagesRecieved();
	void onMessageSended();
};

#endif // VMESSAGES_P_H
