/*
    Vlayer

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

#include "vlayer.h"

Vlayer::Vlayer():
m_vps(VpluginSystem::instance())
{

}

bool Vlayer::init(PluginSystemInterface *plugin_system)
{
        ProtocolInterface::init(plugin_system);
        m_login_widget = 0;
        m_plugin_system = plugin_system;
        m_protocol_icon = new QIcon(":/images/vkontakte.png");
        m_vps.setPluginSystemPointer(m_plugin_system);
        return true;
}

void Vlayer::release()
{
        delete m_login_widget;
        removeProtocolSettings();
        foreach(Vaccount *account, m_account_list)
        {
                killAccount(m_account_list.key(account), false);
        }
}

QWidget *Vlayer::loginWidget()
{
        if ( !m_login_widget )
                m_login_widget = new LoginForm;
        return m_login_widget;
}

void Vlayer::removeLoginWidget()
{
        delete m_login_widget;
        m_login_widget = 0;
}

void Vlayer::saveLoginDataFromLoginWidget(){

        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "vkontaktesettings");
        QStringList accounts = settings.value("accounts/list").toStringList();
        QString account_name = m_login_widget->getName();
        QString account_password = m_login_widget->getPass();
        bool auto_connect = m_login_widget->getAutoConnect();
        if( !accounts.contains(account_name) )
        {
                accounts<<account_name;
                accounts.sort();
                settings.setValue("accounts/list", accounts);
                QSettings account_settings(QSettings::defaultFormat(), QSettings::UserScope,
                                           "qutim/qutim."+m_profile_name+"/vkontakte."+account_name, "accountsettings");
                account_settings.setValue("main/name",account_name);
                account_settings.setValue("main/password",account_password);
                account_settings.setValue("main/autoconnect",auto_connect);
                addAccount(account_name);
        }
}

void Vlayer::addAccount(const QString &account_name)
{
        Vaccount *account = new Vaccount(account_name, m_profile_name);
        account->createAccountButton(m_account_buttons_layout);
        m_account_list.insert(account_name, account);
}

void Vlayer::killAccount(const QString &account_name, bool deleting_account)
{

        Vaccount *delete_account = m_account_list.value(account_name);
        if ( deleting_account )
        {
		delete_account->removeCL();
		m_account_list.remove(account_name);
                delete delete_account;
        }

}

void Vlayer::removeAccount(const QString &account_name)
{
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "vkontaktesettings");
        QStringList accounts = settings.value("accounts/list").toStringList();
        accounts.removeAll(account_name);
        accounts.sort();
        settings.setValue("accounts/list", accounts);
        killAccount(account_name, true);
        QSettings dir_settings_path(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name+"/vkontakte."+account_name, "accountsettings");
        QDir account_dir(dir_settings_path.fileName());
        account_dir.cdUp();

        //delete profile directory
        if( account_dir.exists() )
                removeProfileDir(account_dir.path());
}

void Vlayer::removeProfileDir(const QString &path)
{
        QFileInfo fileInfo(path);
        if( fileInfo.isDir() )
        {
                QDir dir( path );
                QFileInfoList fileList = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
                for (int i = 0; i < fileList.count(); i++)
                        removeProfileDir(fileList.at(i).absoluteFilePath());
                dir.rmdir(path);
        } else {
                QFile::remove(path);
        }
}

void Vlayer::addAccountButtonsToLayout(QHBoxLayout *account_button_layout){

        m_account_buttons_layout = account_button_layout;
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "vkontaktesettings");
        QStringList accountList = settings.value("accounts/list").toStringList();
        foreach( QString account_from_list, accountList)
        {
                addAccount(account_from_list);
        }
}

QList<QMenu*> Vlayer::getAccountStatusMenu()
{
        QList<QMenu*> accounts_menu_list;
        foreach(Vaccount *account, m_account_list)
        {
                accounts_menu_list.append(account->getAccountMenu());
        }
        return accounts_menu_list;
}

QList<AccountStructure> Vlayer::getAccountList()
{
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "vkontaktesettings");
        QList<AccountStructure> accounts_list;
        QStringList accounts = settings.value("accounts/list").toStringList();
        foreach(QString account_name, accounts)
        {
                AccountStructure info_account;
                info_account.protocol_icon = *m_protocol_icon;
                info_account.protocol_name = "VKontakte";
                info_account.account_name = account_name;
                accounts_list.append(info_account);
        }
        return accounts_list;
}

QList<AccountStructure> Vlayer::getAccountStatuses()
{
        m_status_list.clear();
        foreach(Vaccount *account, m_account_list)
        {
                if ( account )
                {
                        AccountStructure info_account;
                        info_account.protocol_icon = account->getCurrentStatusIcon();
                        info_account.protocol_name = "VKontakte";
                        info_account.account_name = m_account_list.key(account);
                        m_status_list.append(info_account);
                }

        }
        return m_status_list;
}

void Vlayer::editAccount(const QString &account){
        if ( m_account_list.contains(account) )
        {
                m_account_list.value(account)->editAccountSettings();
        }
}

QString Vlayer::getItemToolTip(const QString &account_name, const QString &contact_name)
{
    if ( m_account_list.contains(account_name) )
    {
		return m_account_list.value(account_name)->getToolTip(contact_name);
	}
	return QString();
}

void Vlayer::sendMessageTo(const QString &account_name, const QString &contact_name, int /*item_type*/, const QString& message, int /*message_icon_position*/)
{
	if ( m_account_list.contains(account_name) )
	{
		m_account_list.value(account_name)->sendMessage(contact_name, message);
	}
}

QStringList Vlayer::getAdditionalInfoAboutContact(const QString &account_name, const QString &item_name, int /*item_type*/ ) const
{
	if ( m_account_list.contains(account_name) )
	{
		return m_account_list.value(account_name)->getBuddyInfo(item_name);
	}
	return QStringList();
}

void Vlayer::itemContextMenu(const QList<QAction*> &action_list,
				const QString &account_name, const QString &contact_name, int item_type, const QPoint &menu_point)
{
    if ( m_account_list.contains(account_name) )
	{
		m_account_list.value(account_name)->getContactList()->showItemContextMenu(action_list, contact_name, item_type, menu_point);
	}
}

void Vlayer::showContactInformation(const QString &account_name, const QString &item_name, int /*item_type*/ )
{
    if ( m_account_list.contains(account_name) )
	{
		m_account_list.value(account_name)->getContactList()->openPage(item_name);
	}
}

Q_EXPORT_PLUGIN2(vlayer, Vlayer);
