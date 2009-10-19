#include "xsettingsgroup.h"
#include "ui_xsettingsgroup.h"

XSettingsGroup::XSettingsGroup ( const qutim_sdk_0_3::SettingsItemList& settings, QWidget* parent )
: QWidget (parent ), ui (new Ui::XSettingsGroup)
{
	m_setting_list = settings;
	ui->setupUi(this);

	foreach (SettingsItem *settings_item, m_setting_list)
	{
		QListWidgetItem *list_item = new QListWidgetItem (settings_item->icon(),
														  settings_item->text(),
														  ui->listWidget
														  );
	}
	connect(ui->listWidget,SIGNAL(currentRowChanged(int)),SLOT(currentRowChanged(int)));
	ui->listWidget->setCurrentRow(0);
}


void XSettingsGroup::currentRowChanged ( int index)
{
	QWidget *widget = m_setting_list.at(index)->widget();
	if (widget == 0)
		return;
	if (ui->stackedWidget->indexOf(widget) == -1)
		ui->stackedWidget->addWidget(widget);
	ui->stackedWidget->setCurrentWidget(widget);
}


XSettingsGroup::~XSettingsGroup()
{
	delete ui;
}

