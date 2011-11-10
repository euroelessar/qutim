/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Rustam Chakin <qutim.develop@gmail.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

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

