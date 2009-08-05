/*
    GlobalProxySettings

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

#ifndef GLOBALPROXYSETTINGS_H
#define GLOBALPROXYSETTINGS_H

#include <QtGui>
#include "ui_globalproxysettings.h"

class GlobalProxySettings : public QWidget
{
    Q_OBJECT

public:
    GlobalProxySettings(const QString &profile_name, QWidget *parent = 0);
    ~GlobalProxySettings();
    void loadSettings();
    void saveSettings();
    
private slots:
	void widgetStateChanged() { changed = true; emit settingsChanged(); }
	void on_typeBox_2_currentIndexChanged(int type);
    
signals:
  	void settingsChanged();
    void settingsSaved();

private:
    Ui::GlobalProxySettingsClass ui;
    bool changed;
    QString m_profile_name;
};

#endif // GLOBALPROXYSETTINGS_H
