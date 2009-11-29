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

#ifndef YANDEXNARODPLUGIN_H
#define YANDEXNARODPLUGIN_H
#include "qutim/plugininterface.h"
#include <QAction>
#include <QSettings>
#include <QFileDialog>

#include "yandexnarodmanage.h"
#include "yandexnarodsettings.h"
#include "uploaddialog.h"

using namespace qutim_sdk_0_2;

class yandexnarodPlugin : public QObject, SimplePluginInterface, EventHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_2::PluginInterface)
public:
	virtual bool init(PluginSystemInterface *plugin_system);
	virtual void release() {}
	virtual void processEvent(Event &event);
	virtual QWidget *settingsWidget();
	virtual void setProfileName(const QString &profile_name);
	virtual QString name() { return "Yandex.Narod"; }
	virtual QString description() { return "Send files via Yandex.Narod filehosting service"; }
	virtual QString type() { return "other"; }
	virtual QIcon *icon() { return m_plugin_icon; }
	virtual void saveSettings();

private:
	QIcon *m_plugin_icon;
	PluginSystemInterface *m_plugin_system;
	QString m_profile_name;
	QString m_account_name;
	TreeModelItem event_item;
	quint16 event_id;
	uploadDialog* uploadwidget;
	yandexnarodSettings *settingswidget;
	yandexnarodManage* manageDialog;
	yandexnarodNetMan *netman;
	yandexnarodNetMan *testnetman;
	QString msgtemplate;
	QString purl;
	QTime time;
	QFileInfo fi;
	QStringList cooks;
	bool authtest;

private slots:
	void manage_clicked();
	void on_btnTest_clicked();
	void on_TestFinished();
	void actionStart();
	void setCooks(QStringList cs) { cooks = cs; }
	void onFileURL(QString);
	virtual void removeSettingsWidget();

};
#endif
