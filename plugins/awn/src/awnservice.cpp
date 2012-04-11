/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Stanislav (proDOOMman) Kosolapov <prodoomman@shell.tor.hu>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "awnservice.h"
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <QDateTime>
#include <QApplication>

AWNService::AWNService()
{
    m_uread = 0;
    m_item = 0;
    m_activeAccount = 0;
    m_iconTimer = 0;
	//m_cws = 0;
    m_awn = new QDBusInterface("net.launchpad.DockManager",
                              "/net/launchpad/DockManager",
                              "net.launchpad.DockManager");
    QDBusConnection::sessionBus().connect("net.launchpad.DockManager",
                                          "/net/launchpad/DockManager",
                                          "net.launchpad.DockManager",
                                          "ItemRemoved",
                                          this,
                                          SLOT(onItemRemoved(QDBusObjectPath)));
    QDBusMessage mes = m_awn->call("GetCapabilities");
	if(mes.type()==QDBusMessage::ReplyMessage) {
        QDBusReply<QStringList> r;
        r = mes;
        m_capabilities = r.value();
		debug() << "[AWN] dock capabilities: " << m_capabilities;
    }
	else {
		debug() << "[AWN] error: " << mes.errorName() << " : " << mes.errorMessage();
        return;
    }
    if(! m_capabilities.contains("dock-item-icon-file")  ||
       ! m_capabilities.contains("dock-item-message")    ||
       ! m_capabilities.contains("menu-item-with-label") ||
       ! m_capabilities.contains("menu-item-icon-name")    )
    {
        deleteLater();
        return;
    }
    m_icon_size = 128;
    //TODO: а если несколько панелей, как определить на какой находится док?
    QDBusInterface panel("org.awnproject.Awn",
                         "/org/awnproject/Awn/Panel1",
                         "org.awnproject.Awn.Panel");
    QVariant var = panel.property("Size");
    if(var.isValid())
        m_icon_size = var.toInt();
    if(!QDir::temp().exists("qutim-awn"))
        QDir::temp().mkdir("qutim-awn");
    generateIcons();
    m_firstIcon = "qutim";
    m_currentIcon = "qutim";
    m_iconTimer = new QTimer(this);
    connect(m_iconTimer,SIGNAL(timeout()),this,SLOT(nextIcon()));
    m_iconTimer->start(500);
    m_cwc = new ChatWindowController(this);
    qApp->installEventFilter(this);
    connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
    QMap<QString, Protocol*> protocols;
    foreach (Protocol *proto, Protocol::all()) {
        protocols.insert(proto->id(), proto);
        connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
                this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
        foreach(Account *a, proto->accounts())
            emit onAccountCreated(a);
    }
}

AWNService::~AWNService()
{
    if (m_iconTimer)
        m_iconTimer->stop();
	//setDockIcon("qutim"); //WTF ? Oooo fucking code style!!
    foreach(int id, m_menus)
        removeDockMenu(id);
    if(m_awn)
        delete m_awn;
    if(m_item)
        delete m_item;
    QDir t(QDir::tempPath()+"/qutim-awn");
    foreach(QString s,t.entryList(QDir::Files|QDir::NoDotAndDotDot))
        QFile::remove(t.absoluteFilePath(s));
}

bool AWNService::eventFilter(QObject *o, QEvent *event)
{
    if(event->type()==QEvent::Show && !m_item)
    {
        QWidget *w;
        if(QObject *obj = ServiceManager::getByName("ContactList"))
            QMetaObject::invokeMethod(obj,"widget",Qt::DirectConnection,Q_RETURN_ARG(QWidget*,w));
        else
            w = (QWidget*)o;
        w->removeEventFilter(m_cwc);
        w->installEventFilter(m_cwc);
        qlonglong xid = w->effectiveWinId();
        QDBusMessage mes = m_awn->call("GetItemByXid",xid);
        if(mes.type()!=QDBusMessage::ReplyMessage)
			debug() << "[AWN] error: " << mes.errorName() << " : " << mes.errorMessage();
        else
        {
            QDBusReply<QDBusObjectPath> r;
            r = mes;
            setItem(r.value());
        }
    }
    return QObject::eventFilter(o, event);
}

void AWNService::onItemRemoved(QDBusObjectPath path)
{
    if(!m_item||m_item->path()==path.path())
    {
        if(m_item)
            delete m_item;
        m_item = 0;
        if(QObject *obj = ServiceManager::getByName("ContactList"))
        {
            QWidget *w;
            QMetaObject::invokeMethod(obj,"widget",Qt::DirectConnection,Q_RETURN_ARG(QWidget*,w));
            w->showMinimized();
        }
        qApp->installEventFilter(this);
    }
}

void AWNService::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
    connect(session, SIGNAL(unreadChanged(const qutim_sdk_0_3::MessageList&)), SLOT(onUnreadChanged(const qutim_sdk_0_3::MessageList&)));
}

void AWNService::onUnreadChanged(const MessageList &unread)
{
    ChatSession *session = static_cast<ChatSession*>(sender());
    Q_ASSERT(session != NULL);
    if (unread.isEmpty())
        m_sessions.removeOne(session);
    else if (!m_sessions.contains(session))
    {
        m_sessions.append(session);
        if(unread.count()>1)
            for(QList<Message>::const_iterator it = unread.begin(); it<(unread.end()-1); it++)
                session->markRead((*it).id());
    }
    int i = 0;
    foreach(ChatSession *s,m_sessions)
        i += s->unread().count();
    if(i!=m_uread)
    {
        m_uread = i;
        if(i>0)
        {
            m_secondIcon = "mail-unread-new";
            setDockText(QString::number(m_uread));
        }
        else
        {
            m_secondIcon.clear();
            setDockText("");
        }
    }
}

void AWNService::nextIcon()
{
    if(m_currentIcon==m_firstIcon)
    {
        if(!m_secondIcon.isEmpty())
        {
            setDockIcon(m_secondIcon);
            m_currentIcon = m_secondIcon;
        }
    }
    else
    {
        setDockIcon(m_firstIcon);
        m_currentIcon = m_firstIcon;
    }
}

void AWNService::setDockIcon(QString icon_name)
{
    if(!m_item || m_icon_size == -1)
    {
        m_currentIcon.clear();
        return;
    }
    QString file(QDir::tempPath()+QDir::separator()+"qutim-awn%1%2.png");
    file = file.arg(QDir::separator()).arg(icon_name);
    QMap<QString,QVariant> keyval;
    keyval["icon-file"] = file;
    QDBusMessage mes = m_item->call("UpdateDockItem",keyval);
    if(mes.type()==QDBusMessage::ErrorMessage)
        m_currentIcon.clear();
}

void AWNService::setDockText(QString text)
{
    if(!m_item)
        return;
    QMap<QString,QVariant> keyval;
    keyval["message"] = text;
  //keyval["badge"] = text;
    m_item->call("UpdateDockItem",keyval);
}

int AWNService::addDockMenu(QString caption, QString icon_name, QString container)
{
    if(!m_item)
        return -1;
    QMap<QString,QVariant> keyval;
    keyval["label"] = caption;
    if(!container.isEmpty())
        keyval["container-title"] = container;
    if(!icon_name.isEmpty())
        keyval["icon-name"] = icon_name;
    QDBusMessage mes = m_item->call("AddMenuItem",keyval);
    if(mes.type()==QDBusMessage::ReplyMessage)
    {
        QDBusReply<int> r = mes;
        m_menus << r.value();
        return r.value();
    }
	debug() << "[AWN] error: " << mes.errorName() << " : " << mes.errorMessage();
    return -1;
}

void AWNService::removeDockMenu(int id)
{
    if(!m_item)
        return;
    QDBusMessage mes = m_item->call("RemoveMenuItem",id);
    if(mes.type()==QDBusMessage::ErrorMessage)
		debug() << "[AWN] error: " << mes.errorName() << " : " << mes.errorMessage();
    else
        m_menus.removeAll(id);
}

void AWNService::setItem(QDBusObjectPath item_path)
{
    if(m_item)
        delete m_item;
    m_item = new QDBusInterface("net.launchpad.DockManager",
                              item_path.path() ,
                              "net.launchpad.DockItem");
    QDBusConnection::sessionBus().connect("net.launchpad.DockManager",
                                          item_path.path(),
                                          "net.launchpad.DockItem",
                                          "MenuItemActivated",
                                          this,
                                          SLOT(onMenuItemActivated(int)));
    qApp->removeEventFilter(this);
	debug() << "[AWN] finds item path: " << item_path.path();
    m_showhide_menu = addDockMenu(tr("Show/Hide"),"view-refresh");
    m_readall_menu = addDockMenu(tr("Read all messages"),"mail-unread-new");
    m_online_menu = addDockMenu(tr("Go online"),"user-online");
    m_offline_menu = addDockMenu(tr("Go offline"),"user-offline");
    m_exit_menu = addDockMenu(tr("Exit QutIM"),"application-exit");
    for(int i = 1; i < m_showhide_menu; i++)
        removeDockMenu(i);
}

void AWNService::generateIcons()
{
    QDir t(QDir::tempPath()+"/qutim-awn");
    foreach(QString s,t.entryList(QDir::Files|QDir::NoDotAndDotDot))
        QFile::remove(t.absoluteFilePath(s));
    QStringList l;
    l << "qutim" << "mail-unread-new";
    foreach(QString icon_name, l)
        generateIcon(Icon(icon_name),icon_name);
}

QString AWNService::generateIcon(QIcon icon, QString icon_name)
{
    QString file(QDir::tempPath()+QDir::separator()+"qutim-awn%1%2.png");
    file = file.arg(QDir::separator()).arg(icon_name.isEmpty()?"first_icon":icon_name);
    if(!QFile::exists(file)||icon_name.isEmpty())
    {
        QSize s = QSize(m_icon_size,m_icon_size);
        QPixmap p = icon.pixmap(s);
        p.save(file);
    }
    if(icon_name.isEmpty())
        m_currentIcon.clear();
    return(icon_name.isEmpty()?"first_icon":icon_name);
}

void AWNService::onMenuItemActivated(int id)
{
    if(id==m_exit_menu)
    {
        qApp->quit();
    }
    else if(id==m_showhide_menu)
    {
        if(QObject *obj = ServiceManager::getByName("ContactList"))
        {
            QWidget *w;
            QMetaObject::invokeMethod(obj,"widget",Qt::DirectConnection,Q_RETURN_ARG(QWidget*,w));
            if(w->isMinimized())
            {
                w->setWindowState(Qt::WindowActive);
                w->raise();
            }
            else
                w->setWindowState(Qt::WindowMinimized);
        }
    }
    else if(id==m_online_menu)
    {
        foreach(Protocol *p,Protocol::all().values())
            foreach(Account *a, p->accounts())
                a->setStatus(Status::Online);
    }
    else if(id==m_offline_menu)
    {
        foreach(Protocol *p,Protocol::all().values())
            foreach(Account *a, p->accounts())
                a->setStatus(Status::Offline);
    }
    else if(id==m_readall_menu)
    {
        if(!m_sessions.isEmpty())
            foreach(ChatSession *s, m_sessions)
                s->activate();
    }
    else
    {
		debug() << "[AWN] unknown menu item, RIP";
        removeDockMenu(id);
    }
}

void AWNService::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	debug() << "[AWN] account created: " << account->name();
    if (m_accounts.contains(account))
        return;
    m_accounts << account;
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
            this, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
    connect(account, SIGNAL(destroyed(QObject*)), this, SLOT(onAccountDestroyed(QObject*)));
    if (!m_activeAccount) {
        if (account->status().type() != Status::Offline)
            m_activeAccount = account;
        m_firstIcon = generateIcon(account->status().icon());
    }
}

void AWNService::onAccountDestroyed(QObject *obj)
{
    m_accounts.removeAll(static_cast<Account*>(obj));
}

void AWNService::onStatusChanged(const qutim_sdk_0_3::Status &status)
{
    Account *account = qobject_cast<Account*>(sender());
    if (account == m_activeAccount || !m_activeAccount) {
        m_activeAccount = account;
        if (account->status().type() == Status::Offline) {
            m_activeAccount = 0;
        }
        m_firstIcon = generateIcon(status.icon());
    }
    if (!m_activeAccount) {
        foreach (Account *acc, m_accounts) {
            if (acc->status().type() != Status::Offline) {
                m_activeAccount = acc;
                m_firstIcon = generateIcon(status.icon());
                break;
            }
        }
    }
}

