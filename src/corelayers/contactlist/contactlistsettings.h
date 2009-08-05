/*
    ContactListSettings

    Copyright (c) 2008 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef CONTACTLISTSETTINGS_H_
#define CONTACTLISTSETTINGS_H_

#include <QtGui/QWidget>
#include <QList>
#include <QTreeWidgetItem>
#include <QColorDialog>
#include <QSettings>

#include "ui_contactlistsettings.h"

class ContactListSettings : public QWidget
{
    Q_OBJECT
    
public:
	ContactListSettings(const QString &profile_name, QWidget *parent = 0);
	virtual ~ContactListSettings();
    void loadSettings();
    void saveSettings();
public slots:
	void on_accountColorButton_clicked();
	void on_groupColorButton_clicked();
	void on_onlineColorButton_clicked();
	void on_offlineColorButton_clicked();
	void on_separatorColorButton_clicked();
	void onOpacitySliderValueChanged(int value);
    
signals:
	void settingsChanged();
	void settingsSaved();
private:
    QString m_profile_name;
    QColor m_account_col;
    QColor m_group_col;
    QColor m_online_col;
    QColor m_offline_col;
    QColor m_separator_col;
    bool m_gui_changed;

    Ui::ContactListSettingsClass ui;
private slots:
	void widgetSettingsChanged();
	void widgetGuiSettingsChanged();
};

#endif /*CONTACTLISTSETTINGS_H_*/
