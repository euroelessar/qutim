/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
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

#include "historysettings.h"
#include <QSettings>
#include <QDebug>

namespace SqlHistoryNamespace {

SqlHistorySettings::SqlHistorySettings(const QString &profile_name,QWidget *parent)
    : QWidget(parent),
    m_profile_name(profile_name)
{
	ui.setupUi(this);
	changed = false;

	ui.boxSqlEngine->addItem("SQLite");
	ui.boxSqlEngine->addItem("MySQL");
	connect(ui.boxSqlEngine, SIGNAL(currentIndexChanged(int)), this, SLOT(boxSqlEngineChanged(int)));

	loadSettings();

	connect(ui.saveHistoryBox, SIGNAL(stateChanged(int)), this, SLOT(widgetStateChanged()));
	connect(ui.recentBox, SIGNAL(stateChanged(int)), this, SLOT(widgetStateChanged()));
	connect(ui.messagesCountBox, SIGNAL(valueChanged(int)),	this, SLOT(widgetStateChanged()));
	connect(ui.boxSqlEngine, SIGNAL(currentIndexChanged(int)),	this, SLOT(widgetStateChanged()));
	connect(ui.editSqlHost, SIGNAL(textChanged(QString)),	this, SLOT(widgetStateChanged()));
	connect(ui.editSqlPort, SIGNAL(textChanged(QString)),	this, SLOT(widgetStateChanged()));
	connect(ui.editSqlLogin, SIGNAL(textChanged(QString)),	this, SLOT(widgetStateChanged()));
	connect(ui.editSqlPassword, SIGNAL(textChanged(QString)),	this, SLOT(widgetStateChanged()));
	connect(ui.editSqlDBName, SIGNAL(textChanged(QString)),	this, SLOT(widgetStateChanged()));

}

SqlHistorySettings::~SqlHistorySettings()
{

}

void SqlHistorySettings::loadSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "sqlhistory");
	ui.saveHistoryBox->setChecked(settings.value("save", true).toBool());
	ui.recentBox->setChecked(settings.value("showrecent", true).toBool());
	ui.messagesCountBox->setValue(settings.value("recentcount", 4).toUInt());

	if (settings.value("sqlengine", "sqlite").toString()=="sqlite")
		ui.boxSqlEngine->setCurrentIndex(0);
	else if (settings.value("sqlengine").toString()=="mysql") {
		ui.boxSqlEngine->setCurrentIndex(1);
	}

	boxSqlEngineChanged(ui.boxSqlEngine->currentIndex());
}

void SqlHistorySettings::saveSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "sqlhistory");
	settings.setValue("save", ui.saveHistoryBox->isChecked());
	settings.setValue("showrecent", ui.recentBox->isChecked());
	settings.setValue("recentcount", ui.messagesCountBox->value());

	if (ui.boxSqlEngine->currentIndex()==0) settings.setValue("sqlengine", "sqlite");
	else if (ui.boxSqlEngine->currentIndex()==1) {
		settings.setValue("sqlengine", "mysql");
		settings.beginGroup("mysql");
		settings.setValue("host", ui.editSqlHost->text());
		settings.setValue("port", ui.editSqlPort->text());
		settings.setValue("login", ui.editSqlLogin->text());
		settings.setValue("password", ui.editSqlPassword->text());
		settings.setValue("dbname", ui.editSqlDBName->text());
		settings.endGroup();
	}

	if (changed) emit settingsSaved();
	changed = false;
}

void SqlHistorySettings::boxSqlEngineChanged(int idx)
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "sqlhistory");
	if (idx==0) ui.gboxSqlServerSettings->hide();
	else if (idx==1) {
		ui.gboxSqlServerSettings->setTitle("MySQL connection settings");
		ui.gboxSqlServerSettings->show();
		settings.beginGroup("mysql");
		ui.editSqlHost->setText(settings.value("host", "localhost").toString());
		ui.editSqlPort->setText(settings.value("port", "3306").toString());
		ui.editSqlLogin->setText(settings.value("login").toString());
		ui.editSqlPassword->setText(settings.value("password").toString());
		ui.editSqlDBName->setText(settings.value("dbname").toString());
		settings.endGroup();
	}
}

}

