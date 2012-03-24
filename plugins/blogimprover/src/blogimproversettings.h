/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@yandex.ru>
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

#ifndef BLOGIMPROVERSETTINGS_H
#define BLOGIMPROVERSETTINGS_H

#include <qutim/settingswidget.h>
#include "ui_blogimproversettings.h"

class BlogImproverSettings : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT
public:
	BlogImproverSettings();
	~BlogImproverSettings();
protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
signals:
	void reloadSettings();
private:
	Ui::BlogImproverSettingsForm *ui;
};
#endif // BLOGIMPROVERSETTINGS_H
