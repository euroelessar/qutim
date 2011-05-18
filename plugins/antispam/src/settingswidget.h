/****************************************************************************
 *  settingswidget.h
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

#ifndef ANTISPAM_SETTINGSWIDGET_H
#define ANTISPAM_SETTINGSWIDGET_H

#include <QWidget>
#include <qutim/settingswidget.h>

namespace Antispam {

namespace Ui {
    class SettingsWidget;
}

class SettingsWidget : public qutim_sdk_0_3::SettingsWidget
{
    Q_OBJECT
public:
	explicit SettingsWidget();
    ~SettingsWidget();
protected:
	void loadImpl();
	void saveImpl();
	void cancelImpl();
private slots:
	void onModifiedChanged();
private:
    Ui::SettingsWidget *ui;
};


} // namespace Antispam
#endif // ANTISPAM_SETTINGSWIDGET_H
