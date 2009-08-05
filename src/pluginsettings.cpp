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

#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>
#include "pluginsettings.h"


PluginSettings::PluginSettings(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
	setMinimumSize(size());
	moveToDesktopCenter();
	ui.pluginsTree->header()->hide();

	connect(ui.pluginsTree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
			this, SLOT(changeStackWidget(QTreeWidgetItem *)));

	PluginSystem &ps = PluginSystem::instance();
	for(int i = 0; i < ps.pluginsCount(); ++i)
	{
		SimplePluginInterface *plugin = ps.getPluginByIndex(i);
		addPluginItem(plugin);
	}

}


PluginSettings::~PluginSettings()
{
	
}


void PluginSettings::moveToDesktopCenter()
{
	PluginSystem::instance().centerizeWidget(this);
}


void PluginSettings::addPluginItem(SimplePluginInterface *plugin)
{
    if(!plugin)
		return;
	QTreeWidgetItem *item = new QTreeWidgetItem(ui.pluginsTree);
	item->setText(0,plugin->name());
	if( plugin->icon())
		item->setIcon(0, *plugin->icon());
	ui.pluginsStack->addWidget(plugin->settingsWidget());
}


void PluginSettings::changeStackWidget(QTreeWidgetItem *current)
{
	int index = ui.pluginsTree->indexOfTopLevelItem(current);
	ui.pluginsStack->setCurrentIndex(index);
}

void PluginSettings::closeEvent(QCloseEvent * /*event*/)
{
	PluginSystem::instance().removePluginsSettingsWidget();
}

void PluginSettings::on_cancelButton_clicked()
{
	close();
}

void PluginSettings::on_okButton_clicked()
{
	PluginSystem::instance().saveAllPluginsSettings();
	close();
}
