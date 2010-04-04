#include "jmainsettings.h"
#include "ui_jmainsettings.h"
#include "jprotocol.h"

namespace Jabber
{
	JMainSettings::JMainSettings() : ui(new Ui::JMainSettings)
	{
		ui->setupUi(this);
		listenChildrenStates();
	}

	JMainSettings::~JMainSettings()
	{
		delete ui;
	}

	void JMainSettings::loadImpl()
	{
		ConfigGroup general = JProtocol::instance()->config("general");
		ui->resourceEdit->setText(general.value("defaultresource", QString("qutIM")));
		//ui.reconnectCheck->setChecked(settings.value("reconnect",true).toBool());
		ui->avatarRequestCheck->setChecked(!general.value("getavatars", true));
		//ui->transferPostEdit->setValue(settings.value("filetransfer/socks5port", 8010).toInt());
		ConfigGroup priority = JProtocol::instance()->config("priority");
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
		ConfigGroup general = JProtocol::instance()->config("general");
		QString defaultResource = ui->resourceEdit->text().isEmpty() ? "qutIM" : ui->resourceEdit->text();
		general.setValue("defaultresource", defaultResource);
		//ui.reconnectCheck->setChecked(settings.value("reconnect",true).toBool());
		general.setValue("getavatars", !ui->avatarRequestCheck->isChecked());
		//ui->transferPostEdit->setValue(settings.value("filetransfer/socks5port", 8010).toInt());
		general.sync();
		ConfigGroup priority = JProtocol::instance()->config("priority");
		priority.setValue("online", ui->onlinePriority->value());
		priority.setValue("ffchat", ui->ffchatPriority->value());
		priority.setValue("away", ui->awayPriority->value());
		priority.setValue("na", ui->naPriority->value());
		priority.setValue("dnd", ui->dndPriority->value());
		priority.sync();
	}
}
