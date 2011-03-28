#include "wsettings.h"
#include "winint.h"
#include <QTimer>
#include <qutim/config.h>
#include <QSysInfo>

using namespace qutim_sdk_0_3;
using namespace Ui;

WSettingsWidget::WSettingsWidget()
	: SettingsWidget(), ui(new Ui::WSettingsForm)
{
	this->ui->setupUi(this);
	connect(ui->cb_oi_enabled,          SIGNAL(clicked()), SLOT(onCbStateChanged()));
	connect(ui->cb_oi_showNewMsgNumber, SIGNAL(clicked()), SLOT(onCbStateChanged()));
	connect(ui->cb_tt_enabled,          SIGNAL(clicked()), SLOT(onCbStateChanged()));
	connect(this, SIGNAL(saved()), WinIntegration::instance(), SLOT(onSettingsSaved()));
	if (!WinIntegration::instance()->isPluginEnabled(WI_Win7Taskbar))
		ui->tabWidget->removeTab(1); // WARNING: magic numer
}

void WSettingsWidget::loadImpl()
{
	Config cfg(WI_ConfigName);
	if (WinIntegration::instance()->isPluginEnabled(WI_Win7Taskbar)) {
		ui->cb_oi_enabled->           setChecked(cfg.value("oi_enabled",             true));
		ui->cb_oi_addNewConfMsgNumber->setChecked(cfg.value("oi_addNewConfMsgNumber", false));
		ui->cb_oi_showNewMsgNumber->  setChecked(cfg.value("oi_showNewMsgNumber",    true));
		ui->cb_tt_enabled->           setChecked(cfg.value("tt_enabled",             true));
		ui->cb_tt_showNewMsgNumber->  setChecked(cfg.value("tt_showNewMsgCount",     true));
		lookForWidgetState(ui->cb_oi_enabled);
		lookForWidgetState(ui->cb_oi_addNewConfMsgNumber);
		lookForWidgetState(ui->cb_oi_showNewMsgNumber);
		lookForWidgetState(ui->cb_tt_enabled);
		lookForWidgetState(ui->cb_tt_showNewMsgNumber);
	}
	onCbStateChanged();
}

void WSettingsWidget::saveImpl()
{
	Config cfg(WI_ConfigName);
	if (WinIntegration::instance()->isPluginEnabled(WI_Win7Taskbar)) {
		cfg.setValue("oi_enabled",             ui->cb_oi_enabled->            isChecked());
		cfg.setValue("oi_addNewConfMsgNumber", ui->cb_oi_addNewConfMsgNumber->isChecked());
		cfg.setValue("oi_showNewMsgNumber",    ui->cb_oi_showNewMsgNumber->   isChecked());
		cfg.setValue("tt_enabled",             ui->cb_tt_enabled->            isChecked());
		cfg.setValue("tt_showNewMsgNumber",    ui->cb_tt_showNewMsgNumber->   isChecked());
	}
}

void WSettingsWidget::cancelImpl()
{
	loadImpl();
}

void WSettingsWidget::onCbStateChanged()
{
	if (WinIntegration::instance()->isPluginEnabled(WI_Win7Taskbar)) {
		if (!ui->cb_oi_enabled->isChecked()) {
			ui->cb_oi_showNewMsgNumber->   setDisabled(true);
			ui->cb_oi_addNewConfMsgNumber->setDisabled(true);
		} else {
			ui->cb_oi_showNewMsgNumber->setEnabled(true);
			if (!ui->cb_oi_showNewMsgNumber->isChecked())
				ui->cb_oi_addNewConfMsgNumber->setDisabled(true);
			else
				ui->cb_oi_addNewConfMsgNumber->setEnabled(true);
		}

		if (!ui->cb_tt_enabled->isChecked()) {
			ui->cb_tt_showNewMsgNumber->setDisabled(true);
		} else {
			ui->cb_tt_showNewMsgNumber->setEnabled(true);
		}
	}
}
