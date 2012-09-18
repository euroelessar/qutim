/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "vaccountsettings.h"
#include "ui_vaccountsettings.h"
#include "../vaccount.h"
#include <qutim/debug.h>

VAccountSettings::VAccountSettings(QWidget *parent) :
	ui(new Ui::VAccountSettings),m_account(0)
{
	setParent(parent);
    ui->setupUi(this);

	connect(ui->passwdEdit,SIGNAL(textChanged(QString)),SLOT(onPasswdChanged(QString)));
}

VAccountSettings::~VAccountSettings()
{
    delete ui;
}

void VAccountSettings::loadImpl()
{
	Config cfg = m_account->config("general");
	QString password = cfg.value("passwd", QString(), Config::Crypted);
	ui->passwdEdit->setText(password);
}

void VAccountSettings::saveImpl()
{
	Config cfg = m_account->config("general");
	cfg.setValue("passwd",ui->passwdEdit->text(),Config::Crypted);
}

void VAccountSettings::cancelImpl()
{

}

void VAccountSettings::setController(QObject *controller)
{
	m_account = qobject_cast<VAccount*>(controller);
	Q_ASSERT(m_account);
	setWindowTitle(tr("%1 settings").arg(m_account->name()));
}

void VAccountSettings::onPasswdChanged(const QString &)
{
	setModified(true);
}

