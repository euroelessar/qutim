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
: QWidget (parent ), ui (new Ui::XSettingsGroup),m_animated(true)
{
	m_setting_list = settings;
	ui->setupUi(this);
	//appearance
	ConfigGroup general = Config("appearance").group("xsettings/general");
	uint icon_size = general.value<int>("iconSize",16);
	m_animated = general.value<bool>("animated",false);

	ui->listWidget->setIconSize(QSize(icon_size,icon_size));
	ui->stackedWidget->setVerticalMode(true);
	ui->stackedWidget->setSpeed(750);

	QList<int> sizes;
	sizes.append(80);
	sizes.append(250);
	ui->splitter->setSizes(sizes);

	foreach (SettingsItem *settingsItem, m_setting_list) {
		QListWidgetItem *listItem = new QListWidgetItem(settingsItem->icon(),
														settingsItem->text(),
														ui->listWidget);
		listItem->setData(Qt::UserRole, reinterpret_cast<qptrdiff>(settingsItem));
		Q_UNUSED(listItem);
		//list_item->setToolTip(settings_item->description()); //TODO need short description!
	}
	connect(ui->listWidget,SIGNAL(currentRowChanged(int)),SLOT(currentRowChanged(int)));
	currentRowChanged(0);
}

void XSettingsGroup::changeEvent(QEvent *ev)
{
	if (ev->type() == QEvent::LanguageChange) {
		int size = ui->listWidget->count();
		for (int i = 0; i < size; i++) {
			QListWidgetItem *item = ui->listWidget->item(i);
			qptrdiff tmp = item->data(Qt::UserRole).value<qptrdiff>();
			SettingsItem *settingItem = reinterpret_cast<SettingsItem*>(tmp);
			if (settingItem)
				item->setText(settingItem->text());
		}
	}
}

void XSettingsGroup::currentRowChanged ( int index)
{
	SettingsWidget *widget = m_setting_list.at(index)->widget();
	if (widget == 0)
		return;
	if (ui->stackedWidget->indexOf(widget) == -1) {
		widget->setParent(this);
		widget->load();
		ui->stackedWidget->addWidget(widget);
		connect(widget,SIGNAL(modifiedChanged(bool)),SLOT(onWidgetModifiedChanged(bool)));
	}
	if (m_animated)
		ui->stackedWidget->slideInIdx(ui->stackedWidget->indexOf(widget));
	else
		ui->stackedWidget->setCurrentWidget(widget);
	emit titleChanged(m_setting_list.at(index)->text());
}


XSettingsGroup::~XSettingsGroup()
{
}

void XSettingsGroup::onWidgetModifiedChanged(bool haveChanges)
{
	SettingsWidget *widget = qobject_cast< SettingsWidget* >(sender());
	if (!widget)
		return;
	if (haveChanges)
		emit modifiedChanged(widget);
}

