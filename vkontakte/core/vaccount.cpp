/*
    Vaccount

    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "vaccount.h"
#include "edditaccount.h"

Vaccount::Vaccount(const QString &account, const QString &profile_name):
        m_profile_name(profile_name),
        m_account_name(account)
{
    m_edit_dialog_opened = false;
    m_protocol_wrapper = new VprotocolWrap(m_account_name, m_profile_name);
    m_status_object = new VstatusObject(account, profile_name, m_protocol_wrapper, this);
    m_cl_object = new VcontactList(account, profile_name,m_protocol_wrapper);
    connect(m_status_object, SIGNAL(iMOffline()), m_cl_object, SLOT(setAllOffline()));
}

Vaccount::~Vaccount()
{
    delete m_status_object;
    delete m_cl_object;
    delete m_protocol_wrapper;
}

void Vaccount::loadSettings()
{


}

void Vaccount::createAccountButton(QHBoxLayout *layout){
m_status_object->addButtonToLayout(layout);
}

QMenu *Vaccount::getAccountMenu()
{
return m_status_object->getAccountMenu();
}

void Vaccount::editAccountSettings()
{
    if ( !m_edit_dialog_opened )
    {
        EdditAccount *m_edit_dialog = new EdditAccount(m_account_name, m_profile_name);
        connect(m_edit_dialog, SIGNAL(destroyed(QObject*)), this, SLOT(editAccountSettingsClosed(QObject*)));
        m_edit_dialog->show();
        m_edit_dialog_opened = true;
    }
}

void Vaccount::editAccountSettingsClosed(QObject*){
    m_edit_dialog_opened = false;
    m_protocol_wrapper->loadSettings();
    m_cl_object->loadSettings();
}

QIcon Vaccount::getCurrentStatusIcon()
{
    return m_status_object->getCurrentIcon();
}

void Vaccount::removeCL()
{
    m_cl_object->removeCL();
}

QString Vaccount::getToolTip(const QString &buddy_name)
{
    return m_cl_object->getToolTip(buddy_name);
}

void Vaccount::sendMessage(const QString &buddy_id, const QString &message)
{
    m_protocol_wrapper->sendMessage(buddy_id,message);
}

QStringList Vaccount::getBuddyInfo(const QString &buddy_id)
{
    return m_cl_object->getInfoAboutBuddy(buddy_id);
}
