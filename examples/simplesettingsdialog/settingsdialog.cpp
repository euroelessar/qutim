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

