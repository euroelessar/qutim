/*
    qutimSettings

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>
                  2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/


#include <QDebug>
#include "qutimsettings.h"
#include "pluginsettings.h"
#include "pluginsystem.h"
//#include "abstractchatlayer.h"
//#include "abstracthistorylayer.h"
//#include "notifications/abstractnotificationlayer.h"
#include "globalsettings/abstractglobalsettings.h"

qutimSettings::qutimSettings(const QString &profile_name,
		QWidget *parent) :
	QDialog(parent),
	m_iconManager(IconManager::instance())
	, m_profile_name(profile_name)
{
	ui.setupUi(this);
	connect( ui.accountBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(changeProtocolSettings(int)));
	PluginSystem::instance().centerizeWidget(this); // move dialog to desktop center
	setMinimumSize(size());
	ui.settingsTree->header()->hide();
	createSettingsWidget();
	ui.okButton    ->setIcon (m_iconManager.getIcon("apply" ));
	ui.applyButton ->setIcon (m_iconManager.getIcon("apply" ));
	ui.cancelButton->setIcon (m_iconManager.getIcon("cancel"));
	fillProtocolComboBox();
}
qutimSettings::~qutimSettings()
{
	delete msettings;
	delete m_account_management_widget;
	//delete m_chat_window_settings;
//	delete m_anti_spam_settings;
//	delete m_notification_settings;
//	delete m_sound_settings;
//	delete m_history_settings;
	delete m_proxy_settings;
	for(int type = 0;type<InvalidLayer;type++)
    {
		LayerInterface *layer_interface = PluginSystem::instance().getLayerInterface(static_cast<LayerType>(type));
		if(layer_interface)
        {
			layer_interface->removeLayerSettings();
        }
    }
	deleteCurrentProtocolSettings();
}

void qutimSettings::fillProtocolComboBox()
{
	PluginSystem &ps = PluginSystem::instance();
	PluginInfoList protocol_list = ps.getPluginsByType("protocol");
	foreach(PluginInfo information_about_plugin, protocol_list)
	{
		ui.accountBox->addItem(Icon(information_about_plugin.name.toLower(), IconInfo::Protocol),
							   information_about_plugin.name);
	}
}

QPoint qutimSettings::desktopCenter()
{
	QDesktopWidget &desktop = *QApplication::desktop();
	return QPoint(desktop.width() / 2 - size().width() / 2, desktop.height() / 2 - size().height() / 2);
}

void qutimSettings::createSettingsWidget()
{
    m_account_management_item = new QTreeWidgetItem(ui.settingsTree);
    m_account_management_item->setText(0, tr("Accounts"));
    m_account_management_item->setIcon(0, m_iconManager.getIcon("switch_user"));
    m_account_management_widget = new AccountManagement;
    ui.settingsStack->addWidget(m_account_management_widget);

    general = new QTreeWidgetItem(ui.settingsTree);
    general->setText(0, tr("General"));
    general->setIcon(0, m_iconManager.getIcon("mainsettings"));

    msettings = new mainSettings(m_profile_name);
    connect(msettings, SIGNAL(settingsChanged()),
            this, SLOT(enableApply()));
    connect(msettings, SIGNAL(settingsSaved()),
            this, SLOT(mainSettingsChanged()));
    connect(m_account_management_widget, SIGNAL(updateAccountComboBoxFromMainSettings()),
            msettings, SLOT(updateAccountComboBox()));
    ui.settingsStack->addWidget(msettings);
	
//    m_contact_list_settings_item = new QTreeWidgetItem(ui.settingsTree);
//    m_contact_list_settings_item->setText(0, tr("Contact list"));
//    m_contact_list_settings_item->setIcon(0, m_iconManager.getIcon("contactlist"));
//
//    m_contact_list_settings = new ContactListSettings(m_profile_name);
//    ui.settingsStack->addWidget(m_contact_list_settings);
//    connect(m_contact_list_settings, SIGNAL(settingsChanged()),
//            this, SLOT(enableApply()));
//    connect(m_contact_list_settings, SIGNAL(settingsSaved()),
//            this, SLOT(contactListSettingsChanged()));

   /* m_chat_settings_item = new QTreeWidgetItem(ui.settingsTree);
    m_chat_settings_item->setText(0, tr("Chat window"));
    m_chat_settings_item->setIcon(0, m_iconManager.getIcon("messaging"));
*/
    //m_chat_window_settings = new ChatWindowSettings(m_profile_name);
   // ui.settingsStack->addWidget(m_chat_window_settings);
    //connect(m_chat_window_settings, SIGNAL(settingsChanged()),
      //      this, SLOT(enableApply()));
    //connect(m_chat_window_settings, SIGNAL(settingsSaved()),
     //       this, SLOT(chatWindowSettingsChanged()));

/*    m_history_settings_item = new QTreeWidgetItem(ui.settingsTree);
    m_history_settings_item->setText(0, tr("History"));
    m_history_settings_item->setIcon(0, m_iconManager.getIcon("history"));

    m_history_settings = new HistorySettings(m_profile_name);
    ui.settingsStack->addWidget(m_history_settings);
    connect(m_history_settings, SIGNAL(settingsChanged()),
            this, SLOT(enableApply()));
    connect(m_history_settings, SIGNAL(settingsSaved()),
            this, SLOT(historySettingsChanged()));
*/

	for(int type = 0;type<InvalidLayer;type++)
    {
		LayerInterface *layer_interface = PluginSystem::instance().getLayerInterface(static_cast<LayerType>(type));
		if(layer_interface)
	{

			addSettings(layer_interface->getLayerSettingsList());
        }
    }

//	m_notifications_settings_item = new QTreeWidgetItem(ui.settingsTree);
//	m_notifications_settings_item ->setText(0, tr("Notifications"));
//	m_notifications_settings_item ->setIcon(0, m_iconManager.getIcon("events"));

//        m_notification_settings = new NotificationsLayerSettings(m_profile_name);
//        ui.settingsStack->addWidget(m_notification_settings);
//        connect(m_notification_settings, SIGNAL(settingsChanged()),
//                this, SLOT(enableApply()));
//        connect(m_notification_settings, SIGNAL(settingsSaved()),
//                this, SLOT(notificationsSettingsChanged()));

//    m_anti_spam_settings_item = new QTreeWidgetItem(ui.settingsTree);
//    m_anti_spam_settings_item->setText(0, tr("Anti-spam"));
//    m_anti_spam_settings_item->setIcon(0, m_iconManager.getIcon("antispam"));
//
//    m_anti_spam_settings = new AntiSpamLayerSettings(m_profile_name);
//    ui.settingsStack->addWidget(m_anti_spam_settings);
//    connect(m_anti_spam_settings, SIGNAL(settingsChanged()),
//            this, SLOT(enableApply()));
//    connect(m_anti_spam_settings, SIGNAL(settingsSaved()),
//            this, SLOT(antiSpamSettingsChanged()));

//    m_sound_settings_item = new QTreeWidgetItem(ui.settingsTree);
//    m_sound_settings_item->setText(0, tr("Sound"));
//    m_sound_settings_item->setIcon(0, m_iconManager.getIcon("soundsett"));
//
//    m_sound_settings = new SoundLayerSettings(m_profile_name);
//    ui.settingsStack->addWidget(m_sound_settings);
//    connect(m_sound_settings, SIGNAL(settingsChanged()),
//            this, SLOT(enableApply()));
//    connect(m_sound_settings, SIGNAL(settingsSaved()),
//            this, SLOT(soundSettingsChanged()));

    m_global_proxy_item = new QTreeWidgetItem(ui.settingsTree);
    m_global_proxy_item->setText(0, tr("Global proxy"));
    m_global_proxy_item->setIcon(0, m_iconManager.getIcon("proxy"));

    m_proxy_settings = new GlobalProxySettings(m_profile_name);
    ui.settingsStack->addWidget(m_proxy_settings);
    connect(m_proxy_settings, SIGNAL(settingsChanged()),
            this, SLOT(enableApply()));
    connect(m_proxy_settings, SIGNAL(settingsSaved()),
            this, SLOT(globalProxySettingsChanged()));

    ui.settingsTree->resizeColumnToContents(0);

    connect(ui.settingsTree,
            SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
            this, SLOT(changeSettings(QTreeWidgetItem *, QTreeWidgetItem *)));
    ui.settingsTree->setCurrentItem(m_account_management_item);
}

void qutimSettings::addSettings(const QList<SettingsStructure> &settings_list)
{
    foreach( const SettingsStructure &settings_structure, settings_list )
    {
        ui.settingsTree->addTopLevelItem(settings_structure.settings_item);
        QWidget *settings_widget = settings_structure.settings_widget;
        connect( settings_widget, SIGNAL(settingsChanged()), this, SLOT(enableApply()) );
        ui.settingsStack->addWidget(settings_widget);
    }
}

void qutimSettings::changeSettings(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (!current) current = previous;
    ui.settingsStack->setCurrentIndex(ui.settingsTree->indexOfTopLevelItem(current));
}

void qutimSettings::on_okButton_clicked()
{
	if (ui.applyButton->isEnabled() && ui.accountBox->count()) 
		signalForSettingsSaving(ui.accountBox->currentText());
	else if(ui.applyButton->isEnabled())
		signalForSettingsSaving();
	accept();
}

void qutimSettings::on_applyButton_clicked()
{
	if ( ui.accountBox->count() )
		signalForSettingsSaving(ui.accountBox->currentText());
	else
		signalForSettingsSaving();
	ui.applyButton->setEnabled(false);
}

void qutimSettings::saveAllSettings()
{
	msettings->saveSettings();
//	m_contact_list_settings->saveSttings();
//	m_chat_window_settings->saveSettings();
//	m_anti_spam_settings->saveSettings();
//	m_notification_settings->saveSettings();
//	m_sound_settings->saveSettings();
//	m_history_settings->saveSettings();
	m_proxy_settings->saveSettings();
	for(int type = 0;type<InvalidLayer;type++)
	{
		LayerInterface *layer_interface = PluginSystem::instance().getLayerInterface(static_cast<LayerType>(type));
		if(layer_interface)
		{
			layer_interface->saveLayerSettings();
		}
	}
}

void qutimSettings::changeProtocolSettings(int index)
{
	if ( ui.applyButton->isEnabled() )
	{
		QMessageBox msgBox(QMessageBox::NoIcon, tr("Save settings"),
		tr("Save %1 settings?").arg(m_current_account_name), QMessageBox::Yes | QMessageBox::No, this);
		switch( msgBox.exec() )
		{
			case QMessageBox::Yes:
				signalForSettingsSaving(m_current_account_name);
				break;
		
			case QMessageBox::No:
				break;
		
			default:
				break;
		}
	}
	deleteCurrentProtocolSettings();
	
	m_current_account_name = ui.accountBox->itemText(index);
	if ( !m_current_account_name.isEmpty() )
	{
		PluginSystem &ps = PluginSystem::instance();
		QList<SettingsStructure> settings_list = ps.getSettingsByName(m_current_account_name);
                addSettings(settings_list);
	}
}

void qutimSettings::onUpdateTranslation()
{
	general->setText(0, tr("General"));

}

void qutimSettings::signalForSettingsSaving(const QString &protocol_name)
{
	saveAllSettings();
	if(protocol_name.isEmpty())
		return;
	PluginSystem &ps = PluginSystem::instance();
	ps.applySettingsPressed(protocol_name);
}

void qutimSettings::deleteCurrentProtocolSettings()
{
	if( !m_current_account_name.isEmpty() )
	{
		PluginSystem &ps = PluginSystem::instance();
		ps.removeProtocolSettingsByName(m_current_account_name);
	}
}

void qutimSettings::mainSettingsChanged()
{
	AbstractLayer &as = AbstractLayer::instance();
	as.reloadGeneralSettings();
}

void qutimSettings::chatWindowSettingsChanged()
{
//	AbstractChatLayer &acl = AbstractChatLayer::instance();
//	acl.loadSettings();
}

//void qutimSettings::antiSpamSettingsChanged()
//{
//	AbstractAntiSpamLayer &aasl = AbstractAntiSpamLayer::instance();
//	aasl.loadSettings();
//}

//void qutimSettings::notificationsSettingsChanged()
//{
//	AbstractNotificationLayer &anl = AbstractNotificationLayer::instance();
//	anl.loadSettings();
//}

//void qutimSettings::soundSettingsChanged()
//{
//}

//void qutimSettings::contactListSettingsChanged()
//{
////	AbstractContactList::instance().loadSettings();
//}

void qutimSettings::historySettingsChanged()
{
//	AbstractHistoryLayer::instance().loadSettings();
}

void qutimSettings::globalProxySettingsChanged()
{
	AbstractGlobalSettings::instance().loadNetworkSettings();
}
