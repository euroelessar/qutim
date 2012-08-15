#include "docktile.h"
#include <QMenu>
#include <QMetaObject>

#include <qutim/servicemanager.h>
#include <qutim/menucontroller.h>
#include <qutim/chatsession.h>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/icon.h>

using namespace qutim_sdk_0_3;

DockTile::DockTile(QObject *parent) :
    QtDockTile(parent),
    m_menu(new QMenu()),
    m_statusSeparator(0),
    m_sessionSeparator(0),
    m_statusGroup(new QActionGroup(this))
{
    m_statusGroup->setExclusive(true);
    m_statusSeparator = m_menu->addSeparator();

    createStatusAction(Status::Online);
    createStatusAction(Status::FreeChat);
    createStatusAction(Status::Away);
    createStatusAction(Status::DND);
    createStatusAction(Status::NA);
    createStatusAction(Status::Invisible);
    createStatusAction(Status::Offline);

    m_sessionSeparator = m_menu->addSeparator();

    ServicePointer<MenuController> list("ContactList");
    if (list) {
        //foreach (QAction *action, list->menu()->actions())
        //    m_menu->addAction(action);
        platformInvoke("setWidget", list->property("widget"));
    }

    setMenu(m_menu.data());

    connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
            this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
    connect(m_statusGroup, SIGNAL(triggered(QAction*)), SLOT(onStatusTriggered(QAction*)));
}

QAction *DockTile::createStatusAction(Status::Type type)
{
    Status s = Status(type);
    QAction *act = new QAction(s.icon(), s.name(), this);
    act->setData(type);
    m_statusGroup->addAction(act);
    m_menu->insertAction(m_statusSeparator, act);
    return act;
}

void DockTile::onStatusTriggered(QAction *a)
{
    Status::Type type = static_cast<Status::Type>(a->data().value<int>());
    foreach(qutim_sdk_0_3::Protocol *proto, qutim_sdk_0_3::Protocol::all()) {
        foreach(Account *account, proto->accounts()) {
            Status status = account->status();
            status.setType(type);
            status.setSubtype(0);
            status.setProperty("changeReason",Status::ByUser);
            account->setStatus(status);
        }
    }
}

void DockTile::onSessionTriggered()
{
    QAction *action = static_cast<QAction*>(sender());
    ChatSession *session = m_sessions.key(action);
    if (session)
        session->activate();
}

void DockTile::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
    QAction *action = new QAction(Icon("view-choose"), session->unit()->title(), this);
    connect(action, SIGNAL(triggered()), SLOT(onSessionTriggered()));

    m_sessions.insert(session, action);
    m_menu->insertAction(m_sessionSeparator, action);
    setMenu(m_menu.data());

    connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
    connect(session, SIGNAL(destroyed()), SLOT(onSessionDestroyed()));
}

void DockTile::onSessionDestroyed()
{
    ChatSession *session = static_cast<ChatSession*>(sender());
    if (m_sessions.contains(session))
        m_sessions.value(session)->deleteLater();
    m_sessions.remove(session);
}

void DockTile::onUnreadChanged(const qutim_sdk_0_3::MessageList &)
{
    int unread = calculateUnread();
    if (unread)
        setBadge(unread);
    else
        setBadge("");
}

int DockTile::calculateUnread() const
{
    int unread = 0;
    foreach (ChatSession *session, m_sessions.keys()) {
        unread += session->unread().count();
    }
    return unread;
}
