#include "wsettings.h"
#include "winint.h"
#include <QTimer>
#include <qutim/config.h>

using namespace qutim_sdk_0_3;
using namespace Ui;

WSettingsWidget::WSettingsWidget()
	: ui(new Ui::WSettingsForm)
{
	this->ui->setupUi(this);
	connect(ui->cb_oi_enabled,         SIGNAL(clicked()), SLOT(onCbStateChanged()));
	connect(ui->cb_oi_showNewMsgCount, SIGNAL(clicked()), SLOT(onCbStateChanged()));
	connect(ui->cb_tt_enabled,         SIGNAL(clicked()), SLOT(onCbStateChanged()));
}

void WSettingsWidget::loadImpl()
{
	Config cfg(WI_CONFIG);
	ui->cb_oi_enabled->           setChecked(cfg.value("oi_enabled",            true));
	ui->cb_oi_addNewConfMsgCount->setChecked(cfg.value("oi_addNewConfMsgCount", false));
	ui->cb_oi_showNewMsgCount->   setChecked(cfg.value("oi_showNewMsgCount",    true));
	ui->cb_tt_enabled->           setChecked(cfg.value("tt_enabled",            true));
	ui->cb_tt_showLastSenders->   setChecked(cfg.value("tt_showLastSenders",    true));
	ui->cb_tt_showNewMsgCount->   setChecked(cfg.value("tt_showNewMsgCount",    true));
	onCbStateChanged();
	lookForWidgetState(ui->cb_oi_enabled);
	lookForWidgetState(ui->cb_oi_addNewConfMsgCount);
	lookForWidgetState(ui->cb_oi_showNewMsgCount);
	lookForWidgetState(ui->cb_tt_enabled);
	lookForWidgetState(ui->cb_tt_showLastSenders);
	lookForWidgetState(ui->cb_tt_showNewMsgCount);
}

void WSettingsWidget::saveImpl()
{
	Config cfg(WI_CONFIG);
	cfg.setValue("oi_enabled",            ui->cb_oi_enabled->           isChecked());
	cfg.setValue("oi_addNewConfMsgCount", ui->cb_oi_addNewConfMsgCount->isChecked());
	cfg.setValue("oi_showNewMsgCount",    ui->cb_oi_showNewMsgCount->   isChecked());
	cfg.setValue("tt_enabled",            ui->cb_tt_enabled->           isChecked());
	cfg.setValue("tt_showLastSenders",    ui->cb_tt_showLastSenders->   isChecked());
	cfg.setValue("tt_showNewMsgCount",    ui->cb_tt_showNewMsgCount->   isChecked());
	QTimer::singleShot(500, WinIntegration::instance(), SLOT(reloadSettings()));
}

void WSettingsWidget::cancelImpl()
{
	loadImpl();
}

void WSettingsWidget::onCbStateChanged()
{
	if (!ui->cb_oi_enabled->isChecked()) {
		ui->cb_oi_showNewMsgCount->   setDisabled(true);
		ui->cb_oi_addNewConfMsgCount->setDisabled(true);
	} else {
		ui->cb_oi_showNewMsgCount->setEnabled(true);
		if (!ui->cb_oi_showNewMsgCount->isChecked())
			ui->cb_oi_addNewConfMsgCount->setDisabled(true);
		else
			ui->cb_oi_addNewConfMsgCount->setEnabled(true);
	}

	if (!ui->cb_tt_enabled->isChecked()) {
		ui->cb_tt_showLastSenders->setDisabled(true);
		ui->cb_tt_showNewMsgCount->setDisabled(true);
	} else {
		ui->cb_tt_showLastSenders->setEnabled(true);
		ui->cb_tt_showNewMsgCount->setEnabled(true);
	}
}
