/*
    yandexnarodPlugin

	Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "yandexnarod.h"
#include "requestauthdialog.h"

bool yandexnarodPlugin::init(PluginSystemInterface *plugin_system)
{
	qRegisterMetaType<TreeModelItem>("TreeModelItem");

	PluginInterface::init(plugin_system);
	m_plugin_icon = new QIcon(":/icons/yandexnarodplugin.png");
	m_plugin_system = plugin_system;

	msgtemplate = "File sent: %N (%S bytes)\n%U";
	
	return true;
}

void yandexnarodPlugin::processEvent(Event &event)
{
	if (event.id == event_id) {
		event_item = *(TreeModelItem*)(event.args.at(0));
	}
}

QWidget *yandexnarodPlugin::settingsWidget()
{
	settingswidget = new yandexnarodSettings(m_profile_name);
	connect(settingswidget, SIGNAL(testclick()), this,  SLOT(on_btnTest_clicked()));
	return settingswidget;
}

void yandexnarodPlugin::setProfileName(const QString &profile_name)
{
	m_profile_name = profile_name;

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_yandexnarod");
	if (settings.value("main/msgtemplate").isValid()) {
		msgtemplate = settings.value("main/msgtemplate").toString();
	}

	QAction* sendaction = new QAction(QIcon(":/icons/yandexnarodplugin.png"), tr("Send file via Yandex.Narod"), this);
	connect(sendaction, SIGNAL(triggered()), this,  SLOT(actionStart()));
	m_plugin_system->registerContactMenuAction(sendaction);
	event_id = m_plugin_system->registerEventHandler("Core/ContactList/ContactContext", this);

	QAction *manageaction = new QAction(QIcon(":/icons/yandexnarodplugin.png"), tr("Manage Yandex.Narod files"), this);
	m_plugin_system->registerMainMenuAction(manageaction);
	connect(manageaction, SIGNAL(triggered()), this, SLOT(manage_clicked()));
	manageDialog=0;
}

void yandexnarodPlugin::removeSettingsWidget()
{
	delete settingsWidget();
}

void yandexnarodPlugin::saveSettings()
{
	settingswidget->saveSettings();
}

void yandexnarodPlugin::manage_clicked()
{
	manageDialog = new yandexnarodManage(m_profile_name);
	manageDialog->show();
}

void yandexnarodPlugin::on_btnTest_clicked()
{
	testnetman = new yandexnarodNetMan(settingswidget, m_profile_name);
	connect(testnetman, SIGNAL(statusText(QString)), settingswidget, SLOT(setStatus(QString)));
	connect(testnetman, SIGNAL(finished()), this , SLOT(on_TestFinished()));
	testnetman->startAuthTest(settingswidget->getLogin(), settingswidget->getPasswd());
}

void yandexnarodPlugin::on_TestFinished()
{
	delete testnetman;
}

void yandexnarodPlugin::actionStart()
{
	if (!event_item.m_item_name.isEmpty()) {
qDebug()<<"Event item name"<<event_item.m_item_name;
		QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_yandexnarod");
		if (settings.value("main/msgtemplate").isValid()) msgtemplate = settings.value("main/msgtemplate").toString();

		uploadwidget = new uploadDialog();
		connect(uploadwidget, SIGNAL(canceled()), this, SLOT(removeUploadWidget()));

		uploadwidget->show();

		QString filepath = QFileDialog::getOpenFileName(uploadwidget, tr("Choose file for ")+event_item.m_item_name, settings.value("main/lastdir").toString());

		if (filepath.length()>0) {
			fi.setFile(filepath);
			settings.setValue("main/lastdir", fi.dir().path());

			netman = new yandexnarodNetMan(uploadwidget, m_profile_name);
			connect(netman, SIGNAL(statusText(QString)), uploadwidget, SLOT(setStatus(QString)));
			connect(netman, SIGNAL(statusFileName(QString)), uploadwidget, SLOT(setFilename(QString)));
			connect(netman, SIGNAL(transferProgress(qint64,qint64)), uploadwidget, SLOT(progress(qint64,qint64)));
			connect(netman, SIGNAL(uploadFileURL(QString)), this, SLOT(onFileURL(QString)));
			connect(netman, SIGNAL(uploadFinished()), uploadwidget, SLOT(setDone()));
			netman->startUploadFile(filepath);
		}
		else {
			delete uploadwidget; uploadwidget=0;
		}

		authtest=false;
	}
}

void yandexnarodPlugin::onFileURL(QString url)
{
	if (!event_item.m_item_name.isEmpty()) {
		QString sendmsg = msgtemplate;
		sendmsg.replace("%N", fi.fileName());
		sendmsg.replace("%U", url);
		sendmsg.replace("%S", QString::number(fi.size()));
		uploadwidget->setStatus(tr("File sent"));
		uploadwidget->close();
		m_plugin_system->sendCustomMessage(event_item, sendmsg);
		event_item = TreeModelItem();
	}
}

Q_EXPORT_PLUGIN2(yandexnarodPlugin,yandexnarodPlugin);
