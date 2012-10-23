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
	QAction *createStatusAction(Ureen::Status::Type type);
private slots:
	void onStatusTriggered(QAction *);
	void onSessionTriggered();
	void onSessionCreated(Ureen::ChatSession *session);
	void onSessionDestroyed();
	void onUnreadChanged(const Ureen::MessageList &unread);
	int calculateUnread() const;
private:
	QScopedPointer<QMenu> m_menu;
	QAction *m_statusSeparator;
	QAction *m_sessionSeparator;
	QActionGroup *m_statusGroup;
	QHash<Ureen::ChatSession*, QAction*> m_sessions;
};

#endif // DOCKTILE_H
