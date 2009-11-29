/*
	webhistoryPlugin

	Copyright (c) 2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/


#ifndef WEBHISTORYPLUGIN_H
#define WEBHISTORYPLUGIN_H
#include "qutim/plugininterface.h"
#include <QAction>
#include <QSettings>
#include <QDebug>
#include <QtNetwork>
#include <QSqlDatabase>
#include <QSqlQuery>

class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

#include "settings.h"

using namespace qutim_sdk_0_2;

class webhistoryPlugin : public QObject, SimplePluginInterface, EventHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_2::PluginInterface)
public:
	virtual bool init(PluginSystemInterface *plugin_system);
	virtual void release();
	virtual void processEvent(Event &event);
	virtual QWidget *settingsWidget();
	virtual void setProfileName(const QString &profile_name);
	virtual QString name();
	virtual QString description();
	virtual QString type();
	virtual QIcon *icon();
	virtual void saveSettings();

private:
	QIcon *m_plugin_icon;
	PluginSystemInterface *m_plugin_system;
	QString m_profile_name;
	QString m_account_name;
	historySettings *settingswidget;
	QHash<QString, QString> cfg_str;
	QHash<QString, bool> cfg_bool;
	qint16 eventid_receive;
	qint16 eventid_send;
	void insertHistory(int, QString, TreeModelItem);
	QNetworkAccessManager *netman;
	QNetworkRequest netreq;
	QUrl url;
	QString auth;\
	uint lasttime;
	QString m_history_path;
	QDir m_history_dir;
	QString quote(const QString&);
	QString json_quote(const QString&);
	void appendHistoryJSON(QFile &file, const HistoryItem &item);
	void appendHistorySQLite(const HistoryItem &item);
	QSqlDatabase db;
	QString db_filepath;
	uint appended_count_json;
	uint appended_count_sqlite;

private slots:
	virtual void removeSettingsWidget();
	void loadSettings();
	void netmanFinished(QNetworkReply*);
	void netmanSslErrors(QNetworkReply * reply, const QList<QSslError> &) { reply->ignoreSslErrors(); }

};
#endif
