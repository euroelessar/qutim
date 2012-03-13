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
#include "managersettings.h"
#include "ui_managersettings.h"
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <QCheckBox>

ManagerSettings::ManagerSettings() :
	ui(new Ui::ManagerSettings)
{
	ui->setupUi(this);
}

ManagerSettings::~ManagerSettings()
{
	delete ui;
}

void ManagerSettings::loadImpl()
{
	qDeleteAll(m_checkbox_list);
	m_checkbox_list.clear();
	foreach (Protocol *protocol,Protocol::all()) {
		foreach (Account *account,protocol->accounts())
			addAccount(account);
	}
}

void ManagerSettings::saveImpl()
{
	foreach (QCheckBox *box,m_checkbox_list) {
		Account *account = box->property("account").value<Account*>();
		account->config().setValue("autoConnect",box->isChecked());
	}
	Config().sync(); //hack
}

void ManagerSettings::cancelImpl()
{

}

void ManagerSettings::addAccount(Account *account)
{
	QCheckBox *box = new QCheckBox(QString("%1 (%2)").arg(account->name()).arg(account->id()),this);
	bool state = account->config().value("autoConnect", true);
	box->setChecked(state);
	box->setProperty("account",qVariantFromValue(account));
	ui->connectionBox->layout()->addWidget(box);
	m_checkbox_list.append(box);
	connect(box,SIGNAL(stateChanged(int)), SLOT(onCheckedStateChanged(int)));
}

void ManagerSettings::onCheckedStateChanged(int)
{
	emit modifiedChanged(true);
}


