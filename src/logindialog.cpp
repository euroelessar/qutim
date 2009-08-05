/*
    loginDialog

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/


#include "logindialog.h"
#include "pluginsystem.h"

loginDialog::loginDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	notStart = false;
	PluginSystem::instance().centerizeWidget(this);
	setFixedSize(size());
	ui.accountBox->lineEdit()->setMaxLength(12); // max account length

	QRegExp rx("[1-9][0-9]{1,9}");
	QValidator *validator = new QRegExpValidator(rx, this);
	ui.accountBox->lineEdit()->setValidator(validator);

	//if account and password not empty then enable button
	ui.signButton->setEnabled(ui.accountBox->lineEdit()->text() != "" && ui.passwordEdit->text() != "");
	connect( ui.accountBox, SIGNAL(editTextChanged ( const QString &)),
			this, SLOT(signInEnable(const QString &)));
	connect( ui.passwordEdit, SIGNAL(textChanged ( const QString &) ),
				this, SLOT(signInEnable(const QString &)));
	connect( ui.accountBox, SIGNAL(editTextChanged( const QString &)),
					this, SLOT(accountEdit(const QString &)));
	connect( ui.accountBox, SIGNAL(currentIndexChanged( const QString &)),
			this, SLOT(accountChanged(const QString &)));
	IconManager &im =IconManager::instance();
	ui.signButton  ->setIcon(im.getIcon("signin"     ));
	ui.deleteButton->setIcon(im.getIcon("delete_user"));
}

loginDialog::~loginDialog()
{

}

QPoint loginDialog::desktopCenter()
{
	QDesktopWidget &desktop = *QApplication::desktop();
	return QPoint(desktop.width() / 2 - size().width() / 2, desktop.height() / 2 - size().height() / 2);
}

void loginDialog::saveSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "mainsettings");
	settings.setValue("logindialog/showstart", ui.openBox->isChecked());
	saveAccounts();
}

void loginDialog::saveAccounts()
{
	//add new account to account list
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "accounts");
	QStringList accounts = settings.value("accounts/list").toStringList();
	QString curraccount(ui.accountBox->currentText());

	//save current acount settings
	savePersonalSettings(curraccount);

	if(!accounts.contains(curraccount))
	{
		QString curraccount(ui.accountBox->currentText());
		accounts<<(curraccount);
		accounts.sort();
		settings.setValue("accounts/list", accounts);
		if( notStart )
			emit addingAccount(curraccount);
	}

	//set default on app run account
	settings.setValue("accounts/default", accounts.indexOf(curraccount));

}

void loginDialog::savePersonalSettings(const QString &account)
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/ICQ."+account, "account");
	settings.setValue("main/account",account);

	if ( ui.saveBox->isChecked() )
	{
//		bool des3 = settings.value("encryption/pass3DES", false).toBool();
//		EncryptionManager encrypt;
//		settings.setValue("main/password",ui.passwordEdit->text());

		// Encrypt and save password
		QString passwd = ui.passwordEdit->text();
		passwd.truncate(8);
		QByteArray roastedPass;
		for ( int i = 0; i < passwd.length(); i++ )
			roastedPass[i] = passwd.at(i).unicode() ^ crypter[i];
		settings.setValue("main/password",roastedPass);
	} else {
		settings.remove("main/password");
	}
	settings.setValue("main/savepass", ui.saveBox->isChecked());
	settings.setValue("connection/auto", ui.connectBox->isChecked());
	settings.setValue("connection/md5", ui.securBox->isChecked());
}

void loginDialog::loadSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "mainsettings");
	ui.openBox->setChecked(settings.value("logindialog/showstart", true).toBool());
	loadAccounts();
}

void loginDialog::loadAccounts()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "accounts");
	ui.accountBox->addItems(settings.value("accounts/list").toStringList());
	ui.accountBox->setCurrentIndex(settings.value("accounts/default", 0).toInt());
	ui.deleteButton->setEnabled(ui.accountBox->count());
	loadPersonalSettings(ui.accountBox->currentText());
}

void loginDialog::loadPersonalSettings(const QString &account)
{

	if(ui.accountBox->currentText() != "")
	{
		QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/ICQ."+account, "account");
		if(account == settings.value("main/account").toString() )
		{
			QByteArray tmpPass = settings.value("main/password").toByteArray();
			QByteArray roastedPass;
			for ( int i = 0; i < tmpPass.length(); i++ )
				roastedPass[i] = tmpPass.at(i) ^ crypter[i];
			ui.passwordEdit->setText(roastedPass);
			ui.saveBox->setChecked(settings.value("main/savepass", true).toBool());
			ui.connectBox->setChecked(settings.value("connection/auto", true).toBool());
			ui.securBox->setChecked(settings.value("connection/md5", true).toBool());
			ui.deleteButton->setEnabled(true);
		} else {
			ui.accountBox->lineEdit()->clear();
		}
	}
}

void loginDialog::signInEnable(const QString &)
{
	ui.signButton->setEnabled(ui.accountBox->lineEdit()->text() != "" && ui.passwordEdit->text() != "");
}

void loginDialog::accountEdit(const QString & account)
{
	//on account uin editing
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "accounts");
	QStringList accountList = settings.value("accounts/list").toStringList();
	if ( accountList.contains(account) )
		loadPersonalSettings(account);
	else
	{
		ui.passwordEdit->clear();
		ui.saveBox->setChecked(true);
		ui.connectBox->setChecked(true);
		ui.securBox->setChecked(true);
		ui.deleteButton->setEnabled(false);
	}


}

void loginDialog::on_deleteButton_clicked()
{
	QString account(ui.accountBox->currentText());
	QMessageBox msgBox(QMessageBox::NoIcon, tr("Delete account"),
			tr("Delete %1 account?").arg(account), QMessageBox::Yes | QMessageBox::No, this);
	switch( msgBox.exec() )
	{
	case QMessageBox::Yes:
		deleteCurrentAccount(account);
		break;
	case QMessageBox::No:

		break;
	default:
		break;
	}
}

void loginDialog::deleteCurrentAccount(const QString &account)
{
	if ( notStart )
			emit removingAccount(account);
	ui.accountBox->removeItem(ui.accountBox->currentIndex());
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "accounts");
	QStringList accountList = settings.value("accounts/list").toStringList();
	accountList.removeAll(account);

	//delete from account list
	if( accountList.count() )
		settings.setValue("accounts/list", accountList);
	else
		settings.remove("accounts/list");

	if(ui.accountBox->currentText() >= 0)
		settings.setValue("accounts/default", 0);
	else
		settings.remove("accounts/default");

	if( !ui.accountBox->count() ) ui.deleteButton->setEnabled(false);

	QSettings dirSettingsPath(QSettings::defaultFormat(), QSettings::UserScope, "qutim/ICQ."+account, "account");
	QDir accountDir(dirSettingsPath.fileName());
	accountDir.cdUp();

	//delete account directory
	if( accountDir.exists() )
		removeAccountDir(accountDir.path());



}

void loginDialog::removeAccountDir(const QString &path)
{
	//recursively delete all files in directory
	QFileInfo fileInfo(path);
	if( fileInfo.isDir() )
	{
		QDir dir( path );
		QFileInfoList fileList = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
		for (int i = 0; i < fileList.count(); i++)
			removeAccountDir(fileList.at(i).absoluteFilePath());
		dir.rmdir(path);
	} else {
		QFile::remove(path);
	}
}

void loginDialog::on_saveBox_stateChanged(int state)
{
	if ( state )
	{
		ui.passwordEdit->setEnabled(true);
		if ( ui.passwordEdit->text().isEmpty())
			ui.signButton->setEnabled(false);
		else
			ui.signButton->setEnabled(true);

	} else {
		ui.passwordEdit->setEnabled(false);
		ui.signButton->setEnabled(true);
	}
}
