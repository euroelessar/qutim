/****************************************************************************
 *  popupappearance.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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
private:
	void getThemes();
	void preview();
	Ui::AppearanceSettings *ui;
};

} //namespace KineticPopups

#endif // POPUPAPPEARANCE_H
