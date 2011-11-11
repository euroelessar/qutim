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

#ifndef IMAGEPUBSETTINGS_H
#define IMAGEPUBSETTINGS_H

#include "ui_imagepubsettings.h"

#include <QSettings>
#include <QFile>

class imagepubSettings  : public QWidget
{
	Q_OBJECT;

public:
	imagepubSettings(QString);
	~imagepubSettings();
	void saveSettings();

private:
	Ui::imagepubSettingsClass ui;
	QString m_profile_name;

};
#endif

