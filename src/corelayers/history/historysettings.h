/*
    HistorySettings

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

#ifndef HISTORYSETTINGS_H
#define HISTORYSETTINGS_H

#include <QtGui/QWidget>
#include "ui_historysettings.h"

class HistorySettings : public QWidget
{
    Q_OBJECT

public:
    HistorySettings(const QString &profile_name,QWidget *parent = 0);
    ~HistorySettings();
    void loadSettings();
    void saveSettings();
    
private slots:
	void widgetStateChanged() { changed = true; emit settingsChanged(); }
    
signals:
  	void settingsChanged();
    void settingsSaved();

private:
    Ui::HistorySettingsClass ui;
    bool changed;
    QString m_profile_name;
};

#endif // HISTORYSETTINGS_H
