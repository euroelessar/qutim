#include "docktile.h"
#include <QMenu>
#include <qutim/servicemanager.h>
#include <qutim/menucontroller.h>
#include <qutim/chatsession.h>

using namespace qutim_sdk_0_3;

DockTile::DockTile(QObject *parent) :
	QtDockTile(parent),
	m_menu(new QMenu()),
	m_status_separator(0),
	m_session_separator(0)
{
	m_status_separator = m_menu->addSeparator();
	ServicePointer<MenuController> list("ContactList");
	if (list) {
		foreach (QAction *action, list->menu()->actions())
			m_menu->addAction(action);
	}

	setMenu(m_menu.data());

	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
}

QAction *DockTile::createStatusAction(Status::Type type)
{
	Status s = Status(type);
	QAction *act = new QAction(s.icon(), s.name(), this);
	return act;
}

void DockTile::createStatuses(QMenu *parent)
{
}

void DockTile::onStatusTriggered()
{
}

void DockTile::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	m_sessions.append(session);

	connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
	connect(session, SIGNAL(destroyed()), SLOT(onSessionDestroyed()));
}

void DockTile::onSessionDestroyed()
{
	ChatSession *session = static_cast<ChatSession*>(sender());
	m_sessions.removeAll(session);
}

void DockTile::onUnreadChanged(const qutim_sdk_0_3::MessageList &)
{
	setBadge(calculateUnread());
}

int DockTile::calculateUnread() const
{
	int unread = 0;
	foreach (ChatSession *session, m_sessions) {
		unread += session->unread().count();
	}
	return unread;
}
