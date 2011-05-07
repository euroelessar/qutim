/*
	UrlPreviewPlugin

  Copyright (c) 2011 by Nicolay Izoderov <nico-izo@yandex.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef URLPREVIEWSETTINGS_H
#define URLPREVIEWSETTINGS_H

#include <qutim/settingswidget.h>
#include "ui_urlpreviewsettings.h"

class UrlPreviewSettings : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT

public:
	UrlPreviewSettings();
protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
signals:
	void reloadSettings();
private:
        Ui::UrlPreviewSettingsForm *ui;

};
#endif
