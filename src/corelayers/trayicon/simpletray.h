#ifndef SIMPLETRAY_H
#define SIMPLETRAY_H

#include "libqutim/contactlist.h"
#include "libqutim/messagesession.h"
#include "libqutim/account.h"
#include "libqutim/protocol.h"
#include <QSystemTrayIcon>

namespace Core
{
using namespace qutim_sdk_0_3;
class SimpleTray : public MenuController
{
	Q_OBJECT
	Q_CLASSINFO("Service", "TrayIcon")
	Q_CLASSINFO("Uses", "ContactList")
	Q_CLASSINFO("Uses", "IconLoader")
public:
	SimpleTray();

private slots:
	void onActivated(QSystemTrayIcon::ActivationReason);
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onSessionDestroyed();
	void onUnreadChanged(const qutim_sdk_0_3::MessageList &unread);
	void onAccountDestroyed(QObject *obj);
	void onAccountCreated(qutim_sdk_0_3::Account *);
	void onStatusChanged(const qutim_sdk_0_3::Status &);
protected:
	virtual void timerEvent(QTimerEvent *);
private:
	QSystemTrayIcon *m_icon;
	QMap<Account*, ActionGenerator*> m_actions;
	QList<Account*> m_accounts;
	Account *m_activeAccount;
	QList<Protocol*> m_protocols;
	QList<ChatSession*> m_sessions;
	QIcon m_currentIcon;
	int m_iconTimerId;
	QIcon m_mailIcon;
	bool m_isMail;
};
}

#endif // SIMPLETRAY_H
