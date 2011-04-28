#ifndef MACDOCK_H
#define MACDOCK_H

#include <qutim/messagesession.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <QSystemTrayIcon>
#include <QBasicTimer>

namespace MacIntegration
{
using namespace qutim_sdk_0_3;

class MacDockPrivate;
class MacDock : public MenuController
{
	Q_OBJECT
	Q_CLASSINFO("Service", "TrayIcon")
	Q_CLASSINFO("Uses", "ContactList")
	Q_CLASSINFO("Uses", "ChatLayer")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_DECLARE_PRIVATE(MacDock)
public:
	MacDock();
	~MacDock();
	void dockIconClickEvent();
	void loadSettings();
private slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onSessionDestroyed();
	void onUnreadChanged(const qutim_sdk_0_3::MessageList &unread);
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void setStatusIcon();
	void onStatusChanged();
	void onActivatedSession(bool state);
	void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
protected:
	void createDockDeps();
	void removeTrayDeps();
	void createStatusAction(Status::Type type, QMenu *parent);
	void createStatuses(QMenu *parent);
	void createTrayDeps();
	void setBadgeLabel(const QString &string);
	virtual void timerEvent(QTimerEvent *timer);
private:
	QScopedPointer<MacDockPrivate> d_ptr;
};
}

#endif // MACDOCK_H
