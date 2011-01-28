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
	Config general = m_account->config("general");
	ui->resourceEdit->setText(general.value("resource",m_account->client()->jid().resource()));
	ui->avatarRequestCheck->setChecked(!general.value("getAvatars", true));
	ui->passwdEdit->setText(m_account->password());

	Qt::CheckState state = general.value("autoDetect",true) ? Qt::Checked : Qt::Unchecked;
	ui->autodetectBox->setCheckState(state);
	ui->portBox->setValue(general.value("port",5222));
	ui->serverEdit->setText(general.value("server",m_account->client()->server()));

//	general.beginGroup("bosh");
//	general.value("use", false))
//	general.endGroup();

	//ui->transferPostEdit->setValue(settings.value("filetransfer/socks5port", 8010).toInt());
	Config priority = m_account->config("priority");
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
	Config general = m_account->config("general");
	QString defaultResource = ui->resourceEdit->text().isEmpty() ? "qutIM" : ui->resourceEdit->text();
	general.setValue("resource", defaultResource);
	general.setValue("getAvatars", !ui->avatarRequestCheck->isChecked());
	m_account->setPasswd(ui->passwdEdit->text());
	//ui->transferPostEdit->setValue(settings.value("filetransfer/socks5port", 8010).toInt());

	bool autoDetect = ui->autodetectBox->checkState() == Qt::Checked;
	if(!autoDetect) {
		general.setValue("server",ui->serverEdit->text());
		general.setValue("port",ui->portBox->value());
	}
	general.setValue("autoDetect",autoDetect);

	general.sync();
	Config priority = m_account->config("priority");
	priority.setValue("online", ui->onlinePriority->value());
	priority.setValue("ffchat", ui->ffchatPriority->value());
	priority.setValue("away", ui->awayPriority->value());
	priority.setValue("na", ui->naPriority->value());
	priority.setValue("dnd", ui->dndPriority->value());
	priority.sync();
}
}
