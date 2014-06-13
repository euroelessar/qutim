/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ivan Vizir <define-true-false@yandex.com>
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

#include "wsettings.h"
#include "winint.h"
#include <QTimer>
#include <QSysInfo>
#include <QStyle>

#include <qutim/config.h>
#include <qutim/servicemanager.h>

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
	connect(ui->updateAssocs, SIGNAL(clicked()), WinIntegration::instance(), SLOT(updateAssocs()));
	if (!WinIntegration::instance()->isPluginEnabled(WI_Win7Taskbar))
		ui->tabWidget->removeTab(1); // WARNING: magic numer
	if (!ServiceManager::getByName("UriHandler"))
		ui->updateAssocs->setDisabled(true);
	ui->updateAssocs->setIcon(style()->standardIcon(QStyle::SP_VistaShield));
	ui->updateAssocs->hide();
	lookForWidgetState(ui->cb_oi_enabled);
	lookForWidgetState(ui->cb_oi_addNewConfMsgNumber);
	lookForWidgetState(ui->cb_oi_showNewMsgNumber);
	lookForWidgetState(ui->cb_tt_enabled);
	lookForWidgetState(ui->cb_tt_showNewMsgNumber);
}

void WSettingsWidget::loadImpl()
{
	Config cfg(WI_ConfigName);
	if (WinIntegration::instance()->isPluginEnabled(WI_Win7Taskbar)) {
		ui->cb_oi_enabled->            setChecked(cfg.value("oi_enabled",             true));
		ui->cb_oi_addNewConfMsgNumber->setChecked(cfg.value("oi_addNewConfMsgNumber", false));
		ui->cb_oi_showNewMsgNumber->   setChecked(cfg.value("oi_showNewMsgNumber",    true));
		ui->cb_tt_enabled->            setChecked(cfg.value("tt_enabled",             true));
		ui->cb_tt_showNewMsgNumber->   setChecked(cfg.value("tt_showNewMsgCount",     true));
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


