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


#ifndef QUTIMSETTINGS_H
#define QUTIMSETTINGS_H

#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>
#include <QtGui/QDialog>
#include <QMessageBox>
#include "ui_qutimsettings.h"
#include "mainsettings.h"
//#include "pluginsettings.h"
#include "iconmanager.h"
#include "accountmanagement.h"
//#include "chatwindow/chatwindowsettings.h"
//#include "antispam/antispamlayersettings.h"
//#include "notifications/notificationslayersettings.h"
//#include "contactlist/contactlistsettings.h"
//#include "history/historysettings.h"
#include "globalsettings/globalproxysettings.h"

class qutimSettings : public QDialog
{
    Q_OBJECT

public:
    qutimSettings(const QString &profile_name,
    		QWidget *parent = 0);
    ~qutimSettings();
	void applyDisable() { ui.applyButton->setEnabled(false); }
public slots:
	void onUpdateTranslation();
private slots:
	void changeSettings(QTreeWidgetItem *, QTreeWidgetItem *);
	void on_okButton_clicked();
	void on_applyButton_clicked();
	void changeProtocolSettings(int);
	void enableApply() { ui.applyButton->setEnabled(true); }
	void mainSettingsChanged();
	void chatWindowSettingsChanged();
//	void antiSpamSettingsChanged();
//	void notificationsSettingsChanged();
//	void soundSettingsChanged();
//	void contactListSettingsChanged();
	void historySettingsChanged();
	void globalProxySettingsChanged();

signals:
	void generalSettingsChanged();



private:
	void signalForSettingsSaving(const QString &protocol_name="");
	QPoint desktopCenter();
	void saveAllSettings();
        void createSettingsWidget();
        void addSettings(const QList<SettingsStructure> &settings_list);
	void fillProtocolComboBox();
	void deleteCurrentProtocolSettings();

	QSettings *settings;
	QString m_current_account_name;
	QString m_profile_name;
	int currentSettingsIndex;
    Ui::qutimSettingsClass ui;
    mainSettings *msettings;
    //ChatWindowSettings *m_chat_window_settings;
//    AntiSpamLayerSettings *m_anti_spam_settings;
//    NotificationsLayerSettings *m_notification_settings;
//    SoundLayerSettings *m_sound_settings;
//    ContactListSettings *m_contact_list_settings;
    //HistorySettings *m_history_settings;
    GlobalProxySettings *m_proxy_settings;
    QTreeWidgetItem *m_chat_settings_item;
    QTreeWidgetItem *m_anti_spam_settings_item;
    QTreeWidgetItem *m_contact_list_settings_item;
    QTreeWidgetItem *general;
    QTreeWidgetItem *m_account_management_item;
    QTreeWidgetItem *m_notifications_settings_item;
    QTreeWidgetItem *m_sound_settings_item;
    QTreeWidgetItem *m_history_settings_item;
    QTreeWidgetItem *m_global_proxy_item;
    AccountManagement *m_account_management_widget;
    IconManager& m_iconManager;//!< use it to get icons from file or program
};

#endif // QUTIMSETTINGS_H
