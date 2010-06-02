#ifndef VMESSAGES_H
#define VMESSAGES_H

#include <QObject>
#include "vkontakte_global.h"

namespace qutim_sdk_0_3 {
	class Config;
	class Message;
}

class VMessagesPrivate;
class VConnection;

class VMessages : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VMessages)
public:
	VMessages(VConnection *connection,QObject* parent = 0);
	virtual ~VMessages();
	void getLastMessages(int count);
	void sendMessage(const Message &message);
	Config config();
public slots:
	void getHistory();
	void loadSettings();
	void saveSettings();
private:
	QScopedPointer<VMessagesPrivate> d_ptr;
};

#endif // VMESSAGES_H
