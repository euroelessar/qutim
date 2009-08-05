/*
    AntiSpamLayerSettings

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

#include "antispamlayersettings.h"
#include <QSettings>

AntiSpamLayerSettings::AntiSpamLayerSettings(const QString &profile_name,
		QWidget *parent)
    : QWidget(parent),
    m_profile_name(profile_name)
{
	ui.setupUi(this);
	changed = false;
	loadSettings();
	connect(ui.fromClBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.notifyBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.authorizationBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.urlBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.botGroupBox, SIGNAL(toggled(bool)),
				this, SLOT(widgetStateChanged()));
	connect(ui.questionTextEdit, SIGNAL(textChanged()),
				this, SLOT(widgetStateChanged()));
	connect(ui.answerLineEdit, SIGNAL(textChanged(const QString &)),
				this, SLOT(widgetStateChanged()));
	connect(ui.afterAnswerLineEdit, SIGNAL(textChanged(const QString &)),
				this, SLOT(widgetStateChanged()));
	connect(ui.invisibleBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
}

AntiSpamLayerSettings::~AntiSpamLayerSettings()
{

}

void AntiSpamLayerSettings::loadSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("antispam");
	ui.fromClBox->setChecked(settings.value("fromcl", false).toBool());
	ui.notifyBox->setChecked(settings.value("notify", true).toBool());
	ui.authorizationBox->setChecked(settings.value("authorization", false).toBool());
	ui.urlBox->setChecked(settings.value("urls", false).toBool());
	ui.botGroupBox->setChecked(settings.value("botenabled", false).toBool());
	ui.questionTextEdit->setPlainText(settings.value("question", "").toString());
	ui.answerLineEdit->setText(settings.value("answer", "").toString());
	ui.afterAnswerLineEdit->setText(settings.value("afteranswer", "").toString());
	ui.invisibleBox->setChecked(settings.value("oninvisible", false).toBool());
	settings.endGroup();
}

void AntiSpamLayerSettings::saveSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("antispam");
	settings.setValue("fromcl", ui.fromClBox->isChecked());
	settings.setValue("notify", ui.notifyBox->isChecked());
	settings.setValue("authorization", ui.authorizationBox->isChecked());
	settings.setValue("urls", ui.urlBox->isChecked());
	settings.setValue("botenabled", ui.botGroupBox->isChecked());
	settings.setValue("question", ui.questionTextEdit->toPlainText());
	settings.setValue("answer", ui.answerLineEdit->text());
	settings.setValue("afteranswer", ui.afterAnswerLineEdit->text());
	settings.setValue("oninvisible", ui.invisibleBox->isChecked());
	settings.endGroup();
	if ( changed )
			emit settingsSaved();
	changed = false;
}
