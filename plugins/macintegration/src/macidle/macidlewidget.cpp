/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include <qutim/status.h>
#include <qutim/config.h>
#include "macidlewidget.h"
#include "ui_macidlewidget.h"
#include "macidle-global.h"
#include "macidle.h"
#include <QMessageBox>

MacIdleWidget::MacIdleWidget() :
    ui(new Ui::MacIdleWidget)
{
    ui->setupUi(this);
	Status status;
	status.setType(Status::Away);
	ui->awayCheckBox->setText(ui->awayCheckBox->text().arg(status.name().toString()));
	status.setType(Status::NA);
	ui->naCheckBox->setText(ui->naCheckBox->text().arg(status.name().toString()));
}

MacIdleWidget::~MacIdleWidget()
{
    delete ui;
}

void MacIdleWidget::loadImpl()
{
	Config conf = Config(AA_CONFIG_GROUP);
	ui->awayCheckBox->setChecked(conf.value("away-enabled", true));
	ui->naCheckBox->  setChecked(conf.value("na-enabled",   true));
	ui->awaySpinBox->setValue(conf.value("away-secs", AWAY_DEF_SECS)/60);
	ui->naSpinBox->  setValue(conf.value("na-secs",   NA_DEF_SECS)/60);
	ui->awayTextBox->setText(conf.value("away-text", ""));
	ui->naTextBox->  setText(conf.value("na-text",   ""));
	lookForWidgetState(ui->awayCheckBox);
	lookForWidgetState(ui->naCheckBox);
	lookForWidgetState(ui->awaySpinBox);
	lookForWidgetState(ui->naSpinBox);
	lookForWidgetState(ui->awayTextBox);
	lookForWidgetState(ui->naTextBox);
}

void MacIdleWidget::saveImpl()
{
	Config conf = Config(AA_CONFIG_GROUP);
	conf.setValue("away-enabled", ui->awayCheckBox->isChecked());
	conf.setValue("na-enabled",   ui->naCheckBox->  isChecked());
	conf.setValue("away-secs", ui->awaySpinBox->value()*60);
	conf.setValue("na-secs",   ui->naSpinBox->  value()*60);
	conf.setValue("away-text", ui->awayTextBox->toPlainText());
	conf.setValue("na-text",   ui->naTextBox->  toPlainText());
	conf.sync();
	pIdleStatusChanger->reloadSettings();
}

void MacIdleWidget::cancelImpl()
{

}

void MacIdleWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

