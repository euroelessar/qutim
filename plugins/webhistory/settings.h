/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Alexander Kazarin <boiler@co.ru>
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

#ifndef HISTORYSETTINGS_H
#define HISTORYSETTINGS_H

#include "ui_settings.h"

#include <QSettings>
#include <QFileDialog>

class historySettings  : public QWidget
{
	Q_OBJECT;

public:
	historySettings(QString);
	~historySettings();
	void saveSettings();

signals:
	void settingsChanged();

private:
	Ui::historySettingsClass ui;
	QString m_profile_name;

private slots:


};
#endif

