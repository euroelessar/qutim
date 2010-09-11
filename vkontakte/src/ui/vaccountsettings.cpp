#include "vaccountsettings.h"
#include "ui_vaccountsettings.h"
#include "vaccount.h"
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
	emit modifiedChanged(true);
}
