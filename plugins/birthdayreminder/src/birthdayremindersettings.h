/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef BIRTHDAYREMINDERSETTINGS_H
#define BIRTHDAYREMINDERSETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>

namespace Ui {
    class BirthdayReminderSettings;
}

using namespace qutim_sdk_0_3;

class BirthdayReminderSettings : public SettingsWidget
{
    Q_OBJECT
public:
    explicit BirthdayReminderSettings(QWidget *parent = 0);
    ~BirthdayReminderSettings();
protected:
	void loadImpl();
	void saveImpl();
	void cancelImpl();
private:
    Ui::BirthdayReminderSettings *ui;
};

#endif // BIRTHDAYREMINDERSETTINGS_H

