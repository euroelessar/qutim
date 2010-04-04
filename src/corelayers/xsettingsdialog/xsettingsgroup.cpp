/****************************************************************************
 *  xsettingsgroup.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "xsettingsgroup.h"
#include "ui_xsettingsgroup.h"
#include <libqutim/settingswidget.h>
#include <libqutim/configbase.h>
#include <QDebug>

XSettingsGroup::XSettingsGroup ( const qutim_sdk_0_3::SettingsItemList& settings, QWidget* parent )
: QWidget (parent ), ui (new Ui::XSettingsGroup)
{
	m_setting_list = settings;
	ui->setupUi(this);
	//appearance
	ConfigGroup general = Config("appearance").group("xsettings/general");
	uint icon_size = general.value<int>("iconSize",16);
	ui->listWidget->setIconSize(QSize(icon_size,icon_size));

	QList<int> sizes;
	sizes.append(80);
	sizes.append(250);
	ui->splitter->setSizes(sizes);

	foreach (SettingsItem *settings_item, m_setting_list) {
		QListWidgetItem *list_item = new QListWidgetItem (settings_item->icon(),
														  settings_item->text(),
														  ui->listWidget
														  );
		Q_UNUSED(list_item);
		//list_item->setToolTip(settings_item->description()); //TODO need short description!
	}
	connect(ui->listWidget,SIGNAL(currentRowChanged(int)),SLOT(currentRowChanged(int)));
	currentRowChanged(0);
}


void XSettingsGroup::currentRowChanged ( int index)
{
	SettingsWidget *widget = m_setting_list.at(index)->widget();
	m_all_widgets.insert(widget);
	if (widget == 0)
		return;
	if (ui->stackedWidget->indexOf(widget) == -1) {
		widget->load();
		ui->stackedWidget->addWidget(widget);
		connect(widget,SIGNAL(modifiedChanged(bool)),SLOT(onWidgetModifiedChanged(bool)));
	}
	ui->stackedWidget->setCurrentWidget(widget);
	emit titleChanged(m_setting_list.at(index)->text());
}


XSettingsGroup::~XSettingsGroup()
{
	qDeleteAll(m_all_widgets);
}

void XSettingsGroup::onWidgetModifiedChanged(bool haveChanges)
{
	SettingsWidget *widget = qobject_cast< SettingsWidget* >(sender());
	if (!widget)
		return;
	if (haveChanges)
		emit modifiedChanged(widget);
}

