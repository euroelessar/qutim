/*
    mainSettings

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


#ifndef MAINSETTINGS_H
#define MAINSETTINGS_H

#include <QtGui/QWidget>
#include <QSettings>
#include <QDebug>
#include <QMap>
#include <QMapIterator>
#include "pluginsystem.h"

#include "ui_mainsettings.h"

#include "iconmanager.h"

class mainSettings : public QWidget
{
    Q_OBJECT

public:
    mainSettings(const QString &profile_name,
    		QWidget *parent = 0);
    ~mainSettings();
    void loadSettings();
    void saveSettings();

public slots:
	void updateAccountComboBox();
    
private slots:
	void widgetStateChanged() { changed = true; emit settingsChanged(); }


signals:
	void settingsChanged();
    void settingsSaved();

private:
    IconManager& m_iconManager;//!< use it to get icons from file or program
    Ui::mainSettingsClass ui;
    bool changed;
    QString m_current_account_icon;
    QString m_profile_name;
};

#endif // MAINSETTINGS_H
