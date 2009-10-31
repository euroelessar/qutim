/*
    VavatarManagement

    Copyright (c) 2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "vstatusobject.h"
#include "vpluginsystem.h"

VstatusObject::VstatusObject(const QString &account_name, const QString &profile_name,
			     VprotocolWrap *protocol_wrapper, QObject *parent)
	: QObject(parent),
	m_protocol_wrapper(protocol_wrapper),
	m_profile_name(profile_name),
	m_account_name(account_name)
{
    m_status_menu = new QMenu;
    m_status_menu->setTitle(account_name);
    m_online_icon = QIcon(":/images/online.png");
    m_offline_icon = QIcon(":/images/offline.png");
    m_connecting_icon = QIcon(":/images/connecting.png");
    m_status_menu->setIcon(m_offline_icon);
    m_online_action = new QAction(m_online_icon,tr("Online"),this);
    m_offline_action = new QAction(m_offline_icon,tr("Offline"),this);
    m_online_action->setCheckable(true);
    m_offline_action->setCheckable(true);
    m_offline_action->setChecked(true);
    m_account_button = new QToolButton;
    m_account_button->setMinimumSize(QSize(22, 22));
    m_account_button->setMaximumSize(QSize(22, 22));
    m_account_button->setAutoRaise(true);
    m_account_button->setIcon(m_offline_icon);
    m_account_button->setToolTip(account_name);
    m_status_menu->addAction(m_online_action);
    m_status_menu->addAction(m_offline_action);

    m_current_connected = false;

    connect(m_online_action, SIGNAL(triggered()), this, SLOT(connectToServer()));
    connect(m_offline_action, SIGNAL(triggered()), this, SLOT(disconnectFromServer()));
    connect(m_protocol_wrapper, SIGNAL(iMDisconnected()), this, SLOT(wrapperDisconnected()));
    connect(m_protocol_wrapper, SIGNAL(iMConnected()), this, SLOT(wrapperConnected()));

    loadSettings();
}

VstatusObject::~VstatusObject()
{
    delete m_online_action;
    delete m_offline_action;
    delete m_status_menu;
    delete m_account_button;
}

void VstatusObject::loadSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name+"/vkontakte."+m_account_name, "accountsettings");
    if (settings.value("main/autoconnect",false).toBool() )
	connectToServer();
}

void VstatusObject::addButtonToLayout(QHBoxLayout *layout)
{

#if defined(Q_OS_MAC)
        layout->addWidget(accountLineButton, 0, Qt::AlignLeft);
#else
        layout->addWidget(m_account_button, 0, Qt::AlignRight);
#endif

        m_account_button->setPopupMode(QToolButton::InstantPopup);
        m_account_button->setMenu(m_status_menu);
}

QIcon VstatusObject::getCurrentIcon()
{
    return m_account_button->icon();
}

void VstatusObject::connectToServer()
{
    if ( !m_current_connected )
    {
	m_current_connected = true;
	m_account_button->setIcon(m_connecting_icon);
	m_status_menu->setIcon(m_connecting_icon);
	VpluginSystem::instance().updateStatusIcons();
	m_protocol_wrapper->sendAuthorisation();
    }
}

void VstatusObject::disconnectFromServer()
{
    if (m_current_connected)
    {
	m_protocol_wrapper->sendLogout();
	wrapperDisconnected();

    }
}

void VstatusObject::wrapperConnected()
{
	m_online_action->setChecked(true);
	m_offline_action->setChecked(false);
	m_account_button->setIcon(m_online_icon);
	m_status_menu->setIcon(m_online_icon);
	VpluginSystem::instance().updateStatusIcons();
	VpluginSystem::instance().setAccountIsOnline(m_account_name,true);
}

void VstatusObject::wrapperDisconnected()
{

	m_current_connected = false;
	m_online_action->setChecked(false);
	m_offline_action->setChecked(true);
	m_account_button->setIcon(m_offline_icon);
	m_status_menu->setIcon(m_offline_icon);
	VpluginSystem::instance().updateStatusIcons();
	VpluginSystem::instance().setAccountIsOnline(m_account_name,false);
	emit iMOffline();
}
