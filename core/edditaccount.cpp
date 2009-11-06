/*
    EdditAccount

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

#include "edditaccount.h"
#include "ui_edditaccount.h"
#include "vpluginsystem.h"

EdditAccount::EdditAccount(const QString &account, const QString &profile_name,QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::EdditAccount),
    m_profile_name(profile_name),
    m_account_name(account)
{
    m_ui->setupUi(this);
    move(desktopCenter());
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);

    VpluginSystem &vps = VpluginSystem::instance();
    setWindowTitle(tr("Editing %1").arg(account));
    setWindowIcon(vps.getSystemIcon("edituser"));
    m_ui->okButton->setIcon(vps.getSystemIcon("apply"));
    m_ui->applyButton->setIcon(vps.getSystemIcon("apply"));
    m_ui->cancelButton->setIcon(vps.getSystemIcon("cancel"));
    loadSettings();
}

EdditAccount::~EdditAccount()
{
    delete m_ui;
}

void EdditAccount::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QPoint EdditAccount::desktopCenter()
{
				QDesktopWidget &desktop = *QApplication::desktop();
        return QPoint(desktop.width() / 2 - size().width() / 2, desktop.height() / 2 - size().height() / 2);
}

void EdditAccount::loadSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name+"/vkontakte."+m_account_name, "accountsettings");
    m_ui->passEdit->setText(settings.value("main/password","").toString());
    m_ui->autoBox->setChecked(settings.value("main/autoconnect",false).toBool());
    m_ui->statusesBox->setChecked(settings.value("roster/statustext", true).toBool());
    m_ui->friendBox->setValue(settings.value("main/friends", 60).toUInt());
    m_ui->messageBox->setValue(settings.value("main/checkmess", 60).toUInt());
	m_ui->newsBox->setValue(settings.value("main/checknews", 300).toUInt());
	m_ui->cbPhotoEnable->setChecked(settings.value("news/photo_enable",true).toBool());
	m_ui->cbPhotoPreview->setChecked(settings.value("news/photo_preview",true).toBool());
	m_ui->cbPhotoFullsize->setChecked(settings.value("news/photo_fullsize",false).toBool());
}

void EdditAccount::saveSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name+"/vkontakte."+m_account_name, "accountsettings");
    settings.setValue("main/password",m_ui->passEdit->text());
    settings.setValue("main/autoconnect",m_ui->autoBox->isChecked());
    settings.setValue("roster/statustext",m_ui->statusesBox->isChecked());
    settings.setValue("main/friends",m_ui->friendBox->value());
    settings.setValue("main/checkmess", m_ui->messageBox->value());
	settings.setValue("main/checknews", m_ui->newsBox->value());
	settings.setValue("news/photo_enable", m_ui->cbPhotoEnable->isChecked());
	settings.setValue("news/photo_preview", m_ui->cbPhotoPreview->isChecked());
	settings.setValue("news/photo_fullsize", m_ui->cbPhotoFullsize->isChecked());
}

void EdditAccount::on_cancelButton_clicked()
{
    close();
}

void EdditAccount::on_applyButton_clicked()
{
    saveSettings();
}

void EdditAccount::on_okButton_clicked()
{
    saveSettings();
    close();
}
