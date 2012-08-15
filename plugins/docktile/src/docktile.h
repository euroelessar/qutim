#ifndef DOCKTILE_H
#define DOCKTILE_H
#include <qtdocktile.h>
#include <qutim/status.h>
#include <qutim/chatsession.h>

class QAction;

class DockTile : public QtDockTile
{
	Q_OBJECT
public:
	explicit DockTile(QObject *parent = 0);
protected:
	QAction *createStatusAction(qutim_sdk_0_3::Status::Type type);
	void createStatuses(QMenu *parent);
private slots:
	void onStatusTriggered();
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onSessionDestroyed();
	void onUnreadChanged(const qutim_sdk_0_3::MessageList &unread);
	int calculateUnread() const;
private:
	QScopedPointer<QMenu> m_menu;
	QAction *m_status_separator;
	QAction *m_session_separator;
	QList<qutim_sdk_0_3::ChatSession*> m_sessions;
};

#endif // DOCKTILE_H
