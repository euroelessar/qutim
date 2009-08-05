/*
	AbstractTrayLayer

    Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "defaulttraylayer.h"
#include "src/iconmanager.h"
#include "src/pluginsystem.h"
#include <QCoreApplication>

#if defined(Q_OS_MAC)
// macx specific functions, that exported by QT GUI lib
extern void qt_mac_set_dock_menu(QMenu *menu);
#endif

DefaultTrayLayer::DefaultTrayLayer()
{
    m_tray_icon = 0;
    m_tray_menu = 0;
    m_quit_action = 0;
}

bool DefaultTrayLayer::init(PluginSystemInterface *plugin_system)
{
	m_name = "qutim";
	quint8 major, minor, secminor;
	quint16 svn;
	plugin_system->getQutimVersion(major, minor, secminor, svn);
	m_version = QString("%1.%2.%3 r%4").arg(major).arg(minor).arg(secminor).arg(svn);
	createTrayIcon();
	m_event_context = plugin_system->registerEventHandler("Core/Tray/ContextRequested");
	m_event_trigger = plugin_system->registerEventHandler("Core/Tray/Clicked");
	m_event_double_click = plugin_system->registerEventHandler("Core/Tray/DoubleClicked");
	m_event_middle_click = plugin_system->registerEventHandler("Core/Tray/MiddleClicked");
	return true;
}

void DefaultTrayLayer::setProfileName(const QString &)
{
    if(m_quit_action)
    {
        m_quit_action->setText(QObject::tr("&Quit"));
        m_quit_action->setIcon(IconManager::instance().getIcon("exit"));
    }
    if(m_tray_icon)
        m_tray_icon->show();
}

void DefaultTrayLayer::createTrayIcon()
{
    m_tray_menu = new QMenu();

#if !defined(Q_OS_MAC)
    // Mac OS has it's own quit trigger in Dock menu
    m_quit_action = m_tray_menu->addAction(QObject::tr("Quit"), QCoreApplication::instance(), SLOT(quit()));
#endif

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC)
    m_tray_icon = new ExSysTrayIcon();
#else
    m_tray_icon = new QSystemTrayIcon();
#endif

	m_tray_icon->setIcon(IconManager::instance().getIcon("qutim"));

    // It's very unlikely for Mac OS X application to have
    // tray menu and reactions to the tray action at the same time.
    // So we decided to add tray menu to Dock
#if !defined(Q_OS_MAC)
    m_tray_icon->setContextMenu(m_tray_menu);
#else
    qt_mac_set_dock_menu(m_tray_menu);
#endif
	connect(m_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(activated(QSystemTrayIcon::ActivationReason)));
}

void DefaultTrayLayer::release()
{
    if(m_tray_menu)
        delete m_tray_menu;
    if(m_tray_icon)
	{
		m_tray_icon->hide();
		delete m_tray_icon;
	}
	m_tray_menu = 0;
	m_tray_icon = 0;
}

QMenu *DefaultTrayLayer::contextMenu() const
{
    return m_tray_menu;
}

QIcon DefaultTrayLayer::icon() const
{
    if(m_tray_icon)
        return m_tray_icon->icon();
    else return QIcon();
}

void DefaultTrayLayer::setContextMenu(QMenu * menu)
{
    if(m_tray_icon)
        return m_tray_icon->setContextMenu(menu);
}

void DefaultTrayLayer::setIcon(const QIcon & icon)
{
    if(m_tray_icon)
        return m_tray_icon->setIcon(icon);
}

void DefaultTrayLayer::setToolTip(const QString & tip)
{
    if(m_tray_icon)
        return m_tray_icon->setToolTip(tip);
}

void DefaultTrayLayer::showMessage(const QString & title, const QString & message, int timeout_hint)
{
    if(m_tray_icon)
        return m_tray_icon->showMessage(title, message, QSystemTrayIcon::NoIcon, timeout_hint);
}

QString DefaultTrayLayer::toolTip() const
{
    if(m_tray_icon)
        return m_tray_icon->toolTip();
    else
        return "";
}

void DefaultTrayLayer::activated(QSystemTrayIcon::ActivationReason reason)
{
	Event e;
	switch(reason)
	{
	case QSystemTrayIcon::Context:
		e.id = m_event_context;
		break;
	case QSystemTrayIcon::DoubleClick:
		e.id = m_event_double_click;
		break;
	case QSystemTrayIcon::Trigger:
		e.id = m_event_trigger;
		break;
	case QSystemTrayIcon::MiddleClick:
		e.id = m_event_middle_click;
		break;
	default:
		return;
	}
	PluginSystem::instance().sendEvent(e);
}
