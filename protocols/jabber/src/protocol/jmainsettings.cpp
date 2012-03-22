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
#include "jmainsettings.h"
#include "ui_jmainsettings.h"
#include "jprotocol.h"
#include "account/jaccount.h"
#include <jreen/client.h>

namespace Jabber
{
JMainSettings::JMainSettings() : ui(new Ui::JMainSettings)
{
	ui->setupUi(this);
	listenChildrenStates();
#ifdef QUTIM_MOBILE_UI
	ui->formLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
#endif
}

void JMainSettings::setController(QObject *controller)
{
	m_account = qobject_cast<JAccount*>(controller);
}

JMainSettings::~JMainSettings()
{
	delete ui;
}

void JMainSettings::loadImpl()
{
	if(!m_account) //TODO add global config
		return;
	Config general = m_account.data()->config("general");
	ui->resourceEdit->setText(general.value("resource",m_account.data()->client()->jid().resource()));
	ui->avatarRequestCheck->setChecked(!general.value("getAvatars", true));
	ui->passwdEdit->setText(m_account.data()->getPassword());

	Qt::CheckState state = general.value("autoDetect",true) ? Qt::Checked : Qt::Unchecked;
	ui->autodetectBox->setCheckState(state);
	ui->portBox->setValue(general.value("port",5222));
	ui->serverEdit->setText(general.value("server",m_account.data()->client()->server()));

//	general.beginGroup("bosh");
//	general.value("use", false))
//	general.endGroup();

	//ui->transferPostEdit->setValue(settings.value("filetransfer/socks5port", 8010).toInt());
	Config priority = m_account.data()->config("priority");
	ui->onlinePriority->setValue(priority.value("online", 3));
	ui->ffchatPriority->setValue(priority.value("ffchat", 3));
	ui->awayPriority->setValue(priority.value("away", 2));
	ui->naPriority->setValue(priority.value("na", 1));
	ui->dndPriority->setValue(priority.value("dnd", -1));
}

void JMainSettings::cancelImpl()
{
}

void JMainSettings::saveImpl()
{
	Config general = m_account.data()->config("general");
	QString defaultResource = ui->resourceEdit->text().isEmpty() ? "qutIM" : ui->resourceEdit->text();
	general.setValue("resource", defaultResource);
	general.setValue("getAvatars", !ui->avatarRequestCheck->isChecked());
	m_account.data()->setPasswd(ui->passwdEdit->text());
	//ui->transferPostEdit->setValue(settings.value("filetransfer/socks5port", 8010).toInt());

	bool autoDetect = ui->autodetectBox->checkState() == Qt::Checked;
	if(!autoDetect) {
		general.setValue("server",ui->serverEdit->text());
		general.setValue("port",ui->portBox->value());
	}
	general.setValue("autoDetect",autoDetect);

	general.sync();
	Config priority = m_account.data()->config("priority");
	priority.setValue("online", ui->onlinePriority->value());
	priority.setValue("ffchat", ui->ffchatPriority->value());
	priority.setValue("away", ui->awayPriority->value());
	priority.setValue("na", ui->naPriority->value());
	priority.setValue("dnd", ui->dndPriority->value());
	priority.sync();
}
}

