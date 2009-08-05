/*
   ChatLayerSettings

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

#include "chatlayersettings.h"
#include "ui_chatlayersettings.h"

ChatLayerSettings::ChatLayerSettings(const QString &profile_name, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ChatSettingsWidget),
    m_profile_name(profile_name)
{
    m_ui->setupUi(this);
    m_changed = false;
    loadSettings();
    connect(m_ui->tabbedBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->chatConfBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->closeButtonBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->movableBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->rememberBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->closeOnSendBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->openOnNewBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->webkitBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->eventsBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->enterBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->doubleBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->typingBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->blinkBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->showAllBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->removeBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->countSpinBox, SIGNAL(valueChanged(int)), this, SLOT(widgetStateChanged()));
    connect(m_ui->namesBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->timeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetStateChanged()));
    connect(m_ui->colorizeBox, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
}

ChatLayerSettings::~ChatLayerSettings()
{
    delete m_ui;
}

void ChatLayerSettings::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ChatLayerSettings::loadSettings()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
    settings.beginGroup("chatwindow");
    m_ui->tabbedBox->setChecked(settings.value("tabbed", true).toBool());
    m_ui->chatConfBox->setChecked(settings.value("merge", false).toBool());
    m_ui->closeButtonBox->setChecked(settings.value("tabsclosable", true).toBool());
    m_ui->movableBox->setChecked(settings.value("tabsmovable", true).toBool());
    m_ui->rememberBox->setChecked(settings.value("remember", false).toBool());
    m_ui->closeOnSendBox->setChecked(settings.value("close", false).toBool());
    m_ui->openOnNewBox->setChecked(settings.value("open", false).toBool());
    m_ui->webkitBox->setChecked(settings.value("webkit", true).toBool());
    m_ui->eventsBox->setChecked(settings.value("events", false).toBool());
    m_ui->enterBox->setChecked(settings.value("onenter", false).toBool());
	m_ui->doubleBox->setDisabled(m_ui->enterBox->isChecked());
    m_ui->doubleBox->setChecked(settings.value("ondoubleenter", false).toBool());
	if(m_ui->doubleBox->isChecked())
		m_ui->enterBox->setChecked(false);
    m_ui->typingBox->setChecked(settings.value("typing", true).toBool());
    m_ui->blinkBox->setChecked(settings.value("dontblink", false).toBool());
    m_ui->showAllBox->setChecked(settings.value("openall", true).toBool());
    m_ui->removeBox->setChecked(settings.value("remove", false).toBool());
    m_ui->countSpinBox->setValue(settings.value("removecount", 200).toUInt());
    m_ui->timeComboBox->setCurrentIndex(settings.value("timestamp", 1).toUInt());
    m_ui->namesBox->setChecked(settings.value("names", true).toBool());
    m_ui->colorizeBox->setChecked(settings.value("colorize", false).toBool());
    settings.endGroup();
}

void ChatLayerSettings::saveSettings()
{

    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
    settings.beginGroup("chatwindow");
    settings.setValue("tabbed", m_ui->tabbedBox->isChecked());
    settings.setValue("merge", m_ui->chatConfBox->isChecked());
    settings.setValue("tabsclosable", m_ui->closeButtonBox->isChecked());
    settings.setValue("tabsmovable", m_ui->movableBox->isChecked());
    settings.setValue("remember", m_ui->rememberBox->isChecked());
    settings.setValue("close", m_ui->closeOnSendBox->isChecked());
    settings.setValue("open", m_ui->openOnNewBox->isChecked());
    settings.setValue("webkit", m_ui->webkitBox->isChecked());
    settings.setValue("events", m_ui->eventsBox->isChecked());
    settings.setValue("onenter", m_ui->enterBox->isChecked());
    settings.setValue("ondoubleenter", m_ui->doubleBox->isChecked());
    settings.setValue("typing", m_ui->typingBox->isChecked());
    settings.setValue("dontblink", m_ui->blinkBox->isChecked());
    settings.setValue("openall", m_ui->showAllBox->isChecked());
    settings.setValue("remove", m_ui->removeBox->isChecked());
    settings.setValue("removecount", m_ui->countSpinBox->value());
    settings.setValue("timestamp",m_ui->timeComboBox->currentIndex());
    settings.setValue("names",m_ui->namesBox->isChecked());
    settings.setValue("colorize", m_ui->colorizeBox->isChecked());
    settings.endGroup();
    if ( m_changed )
	emit settingsSaved();
    m_changed = false;
}
