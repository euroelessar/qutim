/*
    LoginForm

    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "loginform.h"
#include "ui_loginform.h"


LoginForm::LoginForm(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::LoginForm)
{
    m_ui->setupUi(this);

}

LoginForm::~LoginForm()
{
    delete m_ui;
}

void LoginForm::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QString LoginForm::getName()
{
    return m_ui->emailEdit->text();
}

QString LoginForm::getPass()
{
    return m_ui->passEdit->text();
}

bool LoginForm::getAutoConnect()
{
    return m_ui->autoBox->isChecked();
}
