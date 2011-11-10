/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <qutim/settingswidget.h>

SettingsDialog::SettingsDialog ( const qutim_sdk_0_3::SettingsItemList& settings)
: ui (new Ui::SettingsDialog)
{
	m_setting_list = settings;
	ui->setupUi(this);

	foreach (SettingsItem *settings_item, m_setting_list) //заполняем список настроек самым бесхистростным образом без деления на категории
	{
		QListWidgetItem *list_item = new QListWidgetItem (settings_item->icon(),
														  settings_item->text(),
														  ui->listWidget
														  );
	}
	connect(ui->listWidget,SIGNAL(currentRowChanged(int)),SLOT(currentRowChanged(int)));//навигация по списку настроек
	ui->listWidget->setCurrentRow(0);
}


void SettingsDialog::currentRowChanged ( int index)
{
	SettingsWidget *widget = m_setting_list.at(index)->widget();//генерируем виджет или даем указатель на него, если он существует
	if (widget == 0)
		return;
	if (ui->stackedWidget->indexOf(widget) == -1) //если виджет не добавлен в стек, то добавляем его
	{
		widget->load();
		ui->stackedWidget->addWidget(widget);
	}
	ui->stackedWidget->setCurrentWidget(widget);//просим показать текущий виджет
}


SettingsDialog::~SettingsDialog()
{
	delete ui;
}

