/*
	SqlHistorySettings

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>
				  2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef SQLHISTORYSETTINGS_H
#define SQLHISTORYSETTINGS_H

#include <QtGui/QWidget>
#include "ui_historysettings.h"

namespace SqlHistoryNamespace {

class SqlHistorySettings : public QWidget
{
    Q_OBJECT

public:
	SqlHistorySettings(const QString &profile_name,QWidget *parent = 0);
	~SqlHistorySettings();
    void loadSettings();
    void saveSettings();
    
private slots:
	void widgetStateChanged() { changed = true; emit settingsChanged(); }
	void boxSqlEngineChanged(int);

signals:
  	void settingsChanged();
    void settingsSaved();

private:
	Ui::SqlHistorySettingsClass ui;
    bool changed;
    QString m_profile_name;

};

}

#endif // SQLHISTORYSETTINGS_H
