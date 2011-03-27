/****************************************************************************
 *  pluginchooserwidget.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *  Copyright (c) 2010 by Nikita Belov <null@deltaz.org>
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

#ifndef PLUGINCHOOSERWIDGET_H
#define PLUGINCHOOSERWIDGET_H
#include <qutim/settingswidget.h>
#include <QHash>
#include <QModelIndex>

namespace qutim_sdk_0_3 
{
class ExtensionInfo;
class PluginInfo;
class Plugin;
}

namespace Ui
{
class ServiceChooser;
}

class QStandardItem;
class QStandardItemModel;
namespace Core
{

class ServiceItem;
using namespace qutim_sdk_0_3;

class PluginChooserWidget : public SettingsWidget
{
	Q_OBJECT
public:
	PluginChooserWidget();
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
	virtual ~PluginChooserWidget();
private slots:
	void onItemChanged(QStandardItem*);
	void onItemClicked(QModelIndex index);
private:
	void clear();
	QString html(const qutim_sdk_0_3::PluginInfo& info);
	Ui::ServiceChooser *ui;
	QStandardItemModel *m_model;
	QHash<QString, ServiceItem *> m_plugin_items;
	QHash<QString, Plugin *> m_plugins;
};

}
#endif // PLUGINCHOOSERWIDGET_H
