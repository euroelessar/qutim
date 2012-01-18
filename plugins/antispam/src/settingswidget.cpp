/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "settingswidget.h"
#include "ui_settingswidget.h"
#include <qutim/config.h>

namespace Antispam {

using namespace qutim_sdk_0_3;

SettingsWidget::SettingsWidget() :
	qutim_sdk_0_3::SettingsWidget(),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);

	connect(ui->answerEdit, SIGNAL(textChanged()), SLOT(onModifiedChanged()));
	connect(ui->questionEdit, SIGNAL(textChanged()), SLOT(onModifiedChanged()));
	connect(ui->successEdit, SIGNAL(textChanged()), SLOT(onModifiedChanged()));
	connect(ui->enabledBox, SIGNAL(stateChanged(int)), SLOT(onModifiedChanged()));
	connect(ui->authHandleBox, SIGNAL(stateChanged(int)), SLOT(onModifiedChanged()));
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::loadImpl()
{
	Config cfg;
	cfg.beginGroup(QLatin1String("antispam"));
	ui->enabledBox->setCheckState(cfg.value("enabled", false) ? Qt::Checked : Qt::Unchecked);
	ui->questionEdit->setText(cfg.value("question",  tr("Beer, wine, vodka, champagne: after which drink in this sequence I should stop?")));
	ui->successEdit->setText(cfg.value("success", tr("We are ready to drink with you!")));
	ui->answerEdit->setText(cfg.value("answers", tr("vodka;Vodka")));
	ui->authHandleBox->setCheckState(cfg.value("handleAuth", true) ? Qt::Checked : Qt::Unchecked);
	cfg.endGroup();
}

void SettingsWidget::saveImpl()
{
	Config cfg;
	cfg.beginGroup(QLatin1String("antispam"));
	cfg.setValue("enabled", ui->enabledBox->checkState() == Qt::Checked);
	cfg.setValue("question", ui->questionEdit->toPlainText());
	cfg.setValue("answers", ui->answerEdit->toPlainText());
	cfg.setValue("success", ui->successEdit->toPlainText());
	cfg.setValue("handleAuth", ui->authHandleBox->checkState() == Qt::Checked);
	cfg.endGroup();
}

void SettingsWidget::cancelImpl()
{
	loadImpl();
}

void SettingsWidget::onModifiedChanged()
{
	ui->questionEdit->setEnabled(ui->enabledBox->checkState() == Qt::Checked);
	ui->answerEdit->setEnabled(ui->enabledBox->checkState() == Qt::Checked);
	ui->successEdit->setEnabled(ui->enabledBox->checkState() == Qt::Checked);
	ui->authHandleBox->setEnabled(ui->enabledBox->checkState() == Qt::Checked);
	emit modifiedChanged(true);
}

} // namespace Antispam

