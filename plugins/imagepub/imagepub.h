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


#ifndef IMAGEPUBPLUGIN_H
#define IMAGEPUBPLUGIN_H
#include "qutim/plugininterface.h"
#include <QAction>
#include <QSettings>
#include <QFileDialog>
#include <QtNetwork>

#include "imagepubsettings.h"
#include "uploaddialog.h"

class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

using namespace qutim_sdk_0_2;

class imagepubPlugin : public QObject, SimplePluginInterface, EventHandler
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
	virtual void actionError(QString);

private:
	QIcon *imagepub_plugin_icon;
	PluginSystemInterface *imagepub_plugin_system;
	QString imagepub_profile_name;
	QString imagepub_account_name;
	TreeModelItem event_item;
	quint16 event_id;
	uploadDialog* uploadwidget;
	imagepubSettings *settingswidget;
	QFileInfo fileinfo;
	QString page;
	QNetworkAccessManager *netman;
	QString serv;
	QStringList services;
	QString serv_name;
	QString serv_url;
	QString serv_regexp;
	QString serv_fileinput;
	QString serv_postdata;
	QString serv_filefilter;

private slots:
	void actionStart();
	void netrpFinished( QNetworkReply* );
	virtual void removeSettingsWidget();
	virtual void removeUploadWidget();
};
#endif

