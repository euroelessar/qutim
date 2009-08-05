/*
    AbstractLayer

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "abstractlayer.h"
#include "profilelogindialog.h"
#include "addaccountwizard.h"
#include "qutimsettings.h"
#include "qutim.h"
#include "iconmanager.h"
//#include "abstracthistorylayer.h"
#include "globalsettings/abstractglobalsettings.h"

AbstractLayer::Data *AbstractLayer::d = new AbstractLayer::Data();

AbstractLayer::AbstractLayer()
{
}

AbstractLayer::~AbstractLayer()
{
	
}

AbstractLayer &AbstractLayer::instance()
{
	return *reinterpret_cast<AbstractLayer *>( &SystemsCity::instance() );
}

void AbstractLayer::setPointers(qutIM *parent)
{
	d->parent = parent;
	d->plugin_system = &PluginSystem::instance();
	setPluginSystem( d->plugin_system );
}

bool AbstractLayer::showLoginDialog()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");
	if ( !settings.value("general/showstart", false).toBool() || 
			settings.value("general/switch", false).toBool())
	{
		settings.setValue("general/switch", false);
		ProfileLoginDialog login_dialog;
		if ( login_dialog.exec() )
		{
			d->current_profile = login_dialog.getProfileName();
			d->is_new_profile = login_dialog.isNewProfile();
			return true;
		}
		return false;
	}
	else
	{
		QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");
		QStringList profiles = settings.value("profiles/list").toStringList();
		int profile_index = settings.value("profiles/last", 0).toInt();
                if(profile_index >= profiles.size() || profile_index < 0)
                {
                    settings.setValue("general/switch", true);
                    return showLoginDialog();
                }
				d->current_profile = profiles.at(profile_index);
		return true;
	}
}

void AbstractLayer::loadCurrentProfile()
{
	::IconManager::instance().loadProfile(d->current_profile);
	d->plugin_system->loadProfile(d->current_profile);
	notification()->userMessage(TreeModelItem(), "", NotifyStartup);
	AbstractGlobalSettings::instance().setProfileName(d->current_profile);
}

void AbstractLayer::createNewAccount()
{
	AddAccountWizard wizard;
	wizard.addProtocolsToWizardList(d->plugin_system->getPluginsByType("protocol"));
	if ( wizard.exec() )
	{
		QString protocol_name = wizard.getChosenProtocol();
		if ( !protocol_name.isEmpty() )
		{
			d->plugin_system->saveLoginDataByName(protocol_name);
			d->plugin_system->removeLoginWidgetByName(protocol_name);
		}
		updateStausMenusInTrayMenu();
	} else {
		QString protocol_name = wizard.getChosenProtocol();
		if ( !protocol_name.isEmpty() )
		{
			d->plugin_system->removeLoginWidgetByName(protocol_name);
		}
	}
}

void AbstractLayer::openSettingsDialog()
{

	qutimSettings *settingsDialog = new qutimSettings(d->current_profile);
//	settingsDialog->setIcqList(&icqList);
	QObject::connect ( settingsDialog, SIGNAL(destroyed()),
				d->parent, SLOT(destroySettings()) );
	QObject::connect(d->parent, SIGNAL(updateTranslation()),
		settingsDialog, SLOT(onUpdateTranslation()));

//	settingsDialog->loadAllSettings(); //load all settings to "Settings" window
	settingsDialog->applyDisable(); //disable "Settings" window's Apply button
//	rellocateSettingsDialog(settingsDialog);
	settingsDialog->show();
	settingsDialog->setAttribute(Qt::WA_QuitOnClose, false); //don't close app on "Settings" exit
	settingsDialog->setAttribute(Qt::WA_DeleteOnClose, true);
}

void AbstractLayer::initializePointers(QTreeView *contact_list_view, QHBoxLayout *account_button_layout,
		QMenu *tray_menu, QAction *action_before)
{
	Q_UNUSED(account_button_layout);
	Q_UNUSED(contact_list_view);
//	d->contact_list_management.setTreeView(contact_list_view);
	d->plugin_system->addAccountButtonsToLayout(contactList()->getAccountButtonsLayout());
	d->tray_menu = tray_menu;
	d->action_tray_menu_before = action_before;
	if ( d->is_new_profile )
		createNewAccount();
	//AbstractChatLayer &acl = AbstractChatLayer::instance();
	//acl.restoreData();
	d->plugin_system->pointersAreInitialized();
}

void AbstractLayer::clearMenuFromStatuses()
{
	if ( d->account_status_menu_list.count() )
	{
		foreach(QMenu *account_status_menu, d->account_status_menu_list)
		{
			d->tray_menu->removeAction(account_status_menu->menuAction());
		}
	}
}

void AbstractLayer::addStatusesToMenu()
{
	clearMenuFromStatuses();
	d->account_status_menu_list = d->plugin_system->addStatusMenuToTrayMenu();
	if ( d->account_status_menu_list.count() )
	{
		foreach(QMenu *account_status_menu, d->account_status_menu_list)
		{
			if ( d->action_tray_menu_before && account_status_menu )
				d->tray_menu->insertMenu(d->action_tray_menu_before, account_status_menu);
		}
	}
}

void AbstractLayer::reloadGeneralSettings()
{
	d->parent->reloadGeneralSettings();
	updateTrayIcon();
}

void AbstractLayer::addAccountMenusToTrayMenu(bool add)
{
	if ( add )
	{
		addStatusesToMenu();
	}
	else
	{
		clearMenuFromStatuses();
	}
	d->show_account_menus = add;
}

void AbstractLayer::updateTrayIcon()
{
	QList<AccountStructure> status_list = d->plugin_system->getAccountsStatusesList();
	bool account_founded = false;
	foreach(AccountStructure account_status, status_list)
	{
		if ( d->current_account_icon_name == ( account_status.protocol_name + "." +
				account_status.account_name ) )
		{
			account_founded = true;
			d->parent->updateTrayIcon(account_status.protocol_icon);
		}
	}
	
	if ( !account_founded )
	{
		if ( status_list.count() > 0 )
		{
			d->parent->updateTrayIcon(status_list.at(0).protocol_icon);
		}
		else
		{
			d->parent->updateTrayIcon(Icon("qutim"));
		}
	}
}

void AbstractLayer::updateStausMenusInTrayMenu()
{
	d->account_status_menu_list.clear();
	addAccountMenusToTrayMenu(d->show_account_menus);
}

void AbstractLayer::setAutoAway()
{
	d->plugin_system->setAutoAway();
}

void AbstractLayer::setStatusAfterAutoAway()
{
	d->plugin_system->setStatusAfterAutoAway();
}

void AbstractLayer::animateTrayNewMessage()
{
	d->parent->animateNewMessageInTray();
}

void AbstractLayer::stopTrayNewMessageAnimation()
{
	d->parent->stopNewMessageAnimation();
}

qutIM *AbstractLayer::getParent()
{
	return d->parent;
}

void AbstractLayer::showBalloon(const QString &title, const QString &message, int time)
{
	d->parent->showBallon(title, message, time);
}

void AbstractLayer::reloadStyleLanguage()
{
	d->parent->reloadStyleLanguage();
}

void AbstractLayer::addActionToMainMenu(QAction *action)
{
	d->parent->addActionToList(action);
}
