/*
    AntiSpamLayerSettings

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

#ifndef ANTISPAMLAYERSETTINGS_H
#define ANTISPAMLAYERSETTINGS_H

#include <QtGui/QWidget>
#include "ui_antispamlayersettings.h"

class AntiSpamLayerSettings : public QWidget
{
    Q_OBJECT

public:
	AntiSpamLayerSettings(const QString &profile_name, QWidget *parent = 0);
    ~AntiSpamLayerSettings();
    void loadSettings();
    void saveSettings();

private slots:
	void widgetStateChanged() { changed = true; emit settingsChanged(); }

signals:
  	void settingsChanged();
    void settingsSaved();

private:
    Ui::AntiSpamLayerSettingsClass ui;
    bool changed;
    QString m_profile_name;
};

#endif // ANTISPAMLAYERSETTINGS_H
