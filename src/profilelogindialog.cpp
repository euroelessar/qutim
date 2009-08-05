/*
    ProfileLoginDialog

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

#include "profilelogindialog.h"
#include <QSettings>
#include <QMessageBox>

ProfileLoginDialog::ProfileLoginDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	PluginSystem::instance().centerizeWidget(this);
	setFixedSize(size());
	setWindowTitle(tr("Log in"));
	setWindowIcon(QIcon(":/icons/qutim.png"));
	ui.signButton->setIcon(QIcon(":/icons/core/signin.png"));
	ui.cancelButton->setIcon(QIcon(":/icons/core/cancel.png"));
	ui.deleteButton->setIcon(QIcon(":/icons/core/delete_user.png"));
	m_new_registered_profile = false;
	m_has_new_password = PluginSystem::instance().registerEventHandler("Core/ProfileLogin/PasswordEntered");
	m_has_loged_in = PluginSystem::instance().registerEventHandler("Core/ProfileLogin/LoggedIn");
	loadData();
}

ProfileLoginDialog::~ProfileLoginDialog()
{

}

QPoint ProfileLoginDialog::desktopCenter()
{
	QDesktopWidget &desktop = *QApplication::desktop();
	return QPoint(desktop.width() / 2 - size().width() / 2, desktop.height() / 2 - size().height() / 2);
}

void ProfileLoginDialog::enableOrDisableSignIn()
{
	ui.signButton->setEnabled( !ui.nameComboBox->currentText().isEmpty() && !ui.passwordEdit->text().isEmpty() );
}

void ProfileLoginDialog::on_signButton_clicked()
{
	if(checkPassword())
	{
		saveData();
		m_current_profile = ui.nameComboBox->currentText();
		accept();
	}
	else
		QMessageBox::critical(this, tr("Profile"), tr("Incorrect password"));
}

bool ProfileLoginDialog::checkPassword()
{
    QString password = ui.passwordEdit->text();
    Event event(m_has_new_password, 1, &password);
    PluginSystem::instance().sendEvent(event);
    QString current_profile(ui.nameComboBox->currentText());
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+current_profile, "profilesettings");
	if(!QFileInfo(settings.fileName()).exists())
		return true;
    if(!settings.contains("main/name") || !settings.contains("main/password"))
        return false;
    if(settings.value("main/name").toString() == current_profile)
    {
        if(settings.value("main/password").toString() == ui.passwordEdit->text())
        {
            return true;
        }
        return false;
    }
    return false;
}

void ProfileLoginDialog::saveData()
{
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");
	settings.setValue("general/showstart", ui.showDialogBox->isChecked());
	
	//Save new profile or existed profile's changed options
	
	//add new profile to profile list
	QStringList profiles = settings.value("profiles/list").toStringList();
	QString current_profile(ui.nameComboBox->currentText());

	//save current acount settings
	saveProfileSettings(current_profile);
	qDebug() << profiles << current_profile;

	if( !profiles.contains(current_profile) )
	{
		profiles<<current_profile;
		profiles.sort();
		settings.setValue("profiles/list", profiles);
		m_new_registered_profile = true;
	}

	//set last profile on application run
	settings.setValue("profiles/last", profiles.indexOf(current_profile));
}

void ProfileLoginDialog::saveProfileSettings(const QString &profile_name)
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+profile_name, "profilesettings");
	settings.setValue("main/name",profile_name);
	settings.setValue("main/password",ui.passwordEdit->text());
	settings.setValue("main/savepass", ui.savePasswordBox->isChecked());
}

void ProfileLoginDialog::loadData()
{
	//loading profiles
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");
	ui.nameComboBox->addItems(settings.value("profiles/list").toStringList());
	ui.nameComboBox->setCurrentIndex(settings.value("profiles/last", 0).toInt());
	ui.showDialogBox->setChecked(settings.value("general/showstart", false).toBool());
//	ui.deleteButton->setEnabled(ui.accountBox->count());
	loadProfileSettings(ui.nameComboBox->currentText());
}

void ProfileLoginDialog::loadProfileSettings(const QString &profile_name)
{
	if(ui.nameComboBox->currentText() != "")
	{
		QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+profile_name, "profilesettings");
		if(profile_name == settings.value("main/name").toString() )
		{
			ui.savePasswordBox->setChecked(settings.value("main/savepass", true).toBool());
			if ( ui.savePasswordBox->isChecked() )
			{
				ui.passwordEdit->setText(settings.value("main/password").toString());
			}
			else
			{
				ui.passwordEdit->clear();
			}
//			ui.deleteButton->setEnabled(true);
		} else {
			ui.nameComboBox->lineEdit()->clear();
		}
	}
}

void ProfileLoginDialog::on_nameComboBox_editTextChanged(const QString &profile_name)
{
	enableOrDisableSignIn(); 
	
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");
	QStringList accountList = settings.value("profiles/list").toStringList();
	if ( accountList.contains(profile_name) )
		loadProfileSettings(profile_name);
	else
	{
		ui.passwordEdit->clear();
		ui.savePasswordBox->setChecked(true);
//		ui.deleteButton->setEnabled(false);
	}
}

void ProfileLoginDialog::on_deleteButton_clicked()
{
	QString profile_name(ui.nameComboBox->currentText());
	QMessageBox msgBox(QMessageBox::NoIcon, tr("Delete profile"),
			tr("Delete %1 profile?").arg(profile_name), QMessageBox::Yes | QMessageBox::No, this);
	switch( msgBox.exec() )
	{
	case QMessageBox::Yes:
		removeProfile(profile_name);
		break;
	case QMessageBox::No:

		break;
	default:
		break;
	}
}

void ProfileLoginDialog::removeProfile(const QString &profile_name)
{
	ui.nameComboBox->removeItem(ui.nameComboBox->currentIndex());
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");
	QStringList profiles_list = settings.value("profiles/list").toStringList();
	profiles_list.removeAll(profile_name);

	//delete from profile list
	if( profiles_list.count() )
		settings.setValue("profiles/list", profiles_list);
	else
		settings.remove("profiles/list");

	if(ui.nameComboBox->count() >= 0)
		settings.setValue("profiles/last", 0);
	else
		settings.remove("profiles/last");

	if( !ui.nameComboBox->count() ) ui.deleteButton->setEnabled(false);

	QSettings dir_settings_path(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+profile_name, "profilesettings");
	QDir profile_dir(dir_settings_path.fileName());
	profile_dir.cdUp();

	//delete profile directory
	if( profile_dir.exists() )
		removeProfileDir(profile_dir.path());
}

void ProfileLoginDialog::removeProfileDir(const QString &path)
{
	//recursively delete all files in directory
	QFileInfo fileInfo(path);
	if( fileInfo.isDir() )
	{
		QDir dir( path );
		QFileInfoList fileList = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
		for (int i = 0; i < fileList.count(); i++)
			removeProfileDir(fileList.at(i).absoluteFilePath());
		dir.rmdir(path);
	} else {
		QFile::remove(path);
	}
}
