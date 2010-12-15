#ifndef WIN7INT_H
#define WIN7INT_H

#include <qutim/plugin.h>
#include <qutim/messagesession.h>

class Win7Int2 : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	//Q_CLASSINFO("Service", "ConferenceManager")
	//Q_CLASSINFO("Uses", "ChatLayer")

	QPixmap generateOverlayIcon(quint32 unreadConfs, quint32 unreadChats);

public:
	Win7Int2();
	void init();
	bool load();
	bool unload();

private slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession*);
	void onUnreadChanged(qutim_sdk_0_3::MessageList);

private:

};

#endif // WIN7INT_H
