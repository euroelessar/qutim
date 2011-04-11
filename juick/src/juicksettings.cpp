/*

    Copyright (c) 2011 by Stanislav (proDOOMman) Kosolapov <prodoomman@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include <QColorDialog>
#include "juicksettings.h"
#include "ui_juicksettings.h"
#include <qutim/config.h>

JuickSettings::JuickSettings() :
    ui(new Ui::JuickSettings)
{
    ui->setupUi(this);
    user_color_box = new QtColorComboBox(this);
    foreach(QString cname,QColor::colorNames())
        user_color_box->addColor(QColor(cname),cname);
    ui->user_layout->addWidget(user_color_box);
    post_color_box = new QtColorComboBox(this);
    foreach(QString cname,QColor::colorNames())
        post_color_box->addColor(QColor(cname),cname);
    ui->post_layout->addWidget(post_color_box);
    lookForWidgetState(ui->user_bold);
    lookForWidgetState(ui->user_italic);
    lookForWidgetState(ui->user_under);
    lookForWidgetState(ui->post_bold);
    lookForWidgetState(ui->post_italic);
    lookForWidgetState(ui->post_under);
    lookForWidgetState(ui->extra_cmds);
    lookForWidgetState(user_color_box);
    lookForWidgetState(post_color_box);
    lookForWidgetState(ui->user_line_edit);
    lookForWidgetState(ui->post_line_edit);
}

JuickSettings::~JuickSettings()
{
    delete ui;
}

void JuickSettings::changeEvent(QEvent *e)
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

void JuickSettings::loadImpl()
{
    Config mainGroup = Config("juick").group("main");
    ui->post_bold->setChecked(mainGroup.value("post_bold",false));
    ui->post_italic->setChecked(mainGroup.value("post_italic",false));
    ui->post_under->setChecked(mainGroup.value("post_under",false));
    ui->user_bold->setChecked(mainGroup.value("user_bold",true));
    ui->user_italic->setChecked(mainGroup.value("user_italic",false));
    ui->user_under->setChecked(mainGroup.value("user_under",false));
    user_color_box->setCurrentColor(QColor(mainGroup.value("user_color",QColor(0, 0, 0).name())));
    post_color_box->setCurrentColor(QColor(mainGroup.value("post_color",QColor(0, 128, 0).name())));
    ui->extra_cmds->setChecked(mainGroup.value("extra_cmds",true));
    ui->user_line_edit->setText(mainGroup.value("user_cmds","PM S U"));
    ui->post_line_edit->setText(mainGroup.value("post_cmds","! S D U"));
}

void JuickSettings::saveImpl()
{
    Config mainGroup = Config("juick").group("main");
    mainGroup.setValue("post_bold",ui->post_bold->isChecked());
    mainGroup.setValue("post_italic",ui->post_italic->isChecked());
    mainGroup.setValue("post_under",ui->post_under->isChecked());
    mainGroup.setValue("user_bold",ui->user_bold->isChecked());
    mainGroup.setValue("user_italic",ui->user_italic->isChecked());
    mainGroup.setValue("user_under",ui->user_under->isChecked());
    mainGroup.setValue("user_color",user_color_box->currentColor().name());
    mainGroup.setValue("post_color",post_color_box->currentColor().name());
    mainGroup.setValue("extra_cmds",ui->extra_cmds->isChecked());
    mainGroup.setValue("user_cmds",ui->user_line_edit->text());
    mainGroup.setValue("post_cmds",ui->post_line_edit->text());
}

void JuickSettings::cancelImpl()
{
}
