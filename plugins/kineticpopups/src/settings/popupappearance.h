/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef POPUPAPPEARANCE_H
#define POPUPAPPEARANCE_H
#include <QWidget>
#include "../popupwidgets/quickpopupwidget.h"
#include <qutim/settingswidget.h>

namespace Ui
{
class AppearanceSettings;
}

namespace KineticPopups
{

class AbstractPopupWidget;
class PopupAppearance : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT
public:
	PopupAppearance();
	virtual ~PopupAppearance();
	virtual void loadImpl();
	virtual void cancelImpl();
	virtual void saveImpl();
private slots:
	void onCurrentIndexChanged(int index);
	void onTestButtonClicked();
	void onSpinBoxValueChanged(double);
private:
	void getThemes();
	void preview();
	Ui::AppearanceSettings *ui;
};

} //namespace KineticPopups

#endif // POPUPAPPEARANCE_H

