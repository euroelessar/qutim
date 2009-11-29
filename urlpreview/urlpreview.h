/*
    urlpreviewPlugin

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


#ifndef urlpreviewPLUGIN_H
#define urlpreviewPLUGIN_H
#include "qutim/plugininterface.h"
#include <QAction>
#include <QSettings>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QDebug>

#include "settings.h"

using namespace qutim_sdk_0_2;

class urlpreviewPlugin : public QObject, SimplePluginInterface, EventHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_2::PluginInterface)
public:
	virtual bool init(PluginSystemInterface *plugin_system);
	virtual void release() {}
	virtual void processEvent(Event &event);
	virtual QWidget *settingsWidget();
	virtual void setProfileName(const QString &profile_name);
	virtual QString name() { return "URLPreview"; }
	virtual QString description() { return tr("Make URL previews in messages"); }
	virtual QString type() { return "other"; }
	virtual QIcon *icon();
	virtual void saveSettings();

private:
	QIcon *m_plugin_icon;
	PluginSystemInterface *m_plugin_system;
	QString m_profile_name;
	QString m_account_name;
	urlpreviewSettings *settingswidget;
	QHash<QString, QString> cfg_str;
	bool cfg_enable_in;
	bool cfg_enable_out;
	bool cfg_enable_youtube;
        bool cfg_disable_text_html;
        bool cfg_ignore_sslwarn;
	qint64 cfg_image_maxfilesize;
	qint16 m_event_msgin;
	qint16 m_event_msgout;
	QNetworkAccessManager *netman;
	QHash<QString, QString> uidurls;
	QHash<QString, QString> urluids;
	QHash<QString, TreeModelItem> uiditems;
        void printStrToUID(QString, QString);

private slots:
	virtual void removeSettingsWidget();
	void loadSettings();
        void netmanFinished ( QNetworkReply * );
        void authenticationRequired ( QNetworkReply * , QAuthenticator * );
        void netmanSslErrors ( QNetworkReply * , const QList<QSslError> & );

};
#endif
