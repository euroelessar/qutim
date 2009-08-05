/*****************************************************************************
    Plugin System

    Copyright (c) 2008 by m0rph <m0rph.mailbox@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef PLUGINSETTINGS_H
#define PLUGINSETTINGS_H

#include <QDialog>
#include "ui_pluginsettings.h"
#include "pluginsystem.h"


class PluginSettings : public QDialog
{
	Q_OBJECT;

public:
	PluginSettings(QWidget *parent = 0);
	~PluginSettings();

private:
	Ui::PluginSettingsClass ui;

	void moveToDesktopCenter();
	void addPluginItem(SimplePluginInterface  *plugin);

private slots:
	void changeStackWidget(QTreeWidgetItem *current);
	void on_cancelButton_clicked();
	void on_okButton_clicked();
	
protected:
	void closeEvent(QCloseEvent * event);
};



#endif
