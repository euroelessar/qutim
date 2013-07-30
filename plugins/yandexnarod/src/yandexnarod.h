/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef YANDEXNARODPLUGIN_H
#define YANDEXNARODPLUGIN_H

#include "yandexnarodsettings.h"
#include "yandexnarodauthorizator.h"

#include <qutim/plugin.h>
#include <qutim/filetransfer.h>
#include <qutim/status.h>
#include <qutim/account.h>

#include <QAction>
#include <QFileDialog>

class YandexNarodPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qutim.Plugin")
	Q_CLASSINFO("DebugInfo", "Yandex")
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();

private:
	void loadCookies();

private slots:
	void saveCookies();
};

class YandexNarodFactory : public qutim_sdk_0_3::FileTransferFactory
{
	Q_OBJECT
public:
	YandexNarodFactory();
	virtual bool checkAbility(qutim_sdk_0_3::ChatUnit *unit);
	virtual bool startObserve(qutim_sdk_0_3::ChatUnit *unit);
	virtual bool stopObserve(qutim_sdk_0_3::ChatUnit *unit);
	virtual qutim_sdk_0_3::FileTransferJob *create(qutim_sdk_0_3::ChatUnit *unit);
	static QNetworkAccessManager *networkManager();
	static YandexNarodAuthorizator *authorizator();
private slots:
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status);
	void onAccountAdded(qutim_sdk_0_3::Account *account);
private:
	typedef QMultiMap<QObject*, qutim_sdk_0_3::ChatUnit*> Observers;
	Observers m_observedUnits;
};

class YandexRequest : public QNetworkRequest
{
public:
	YandexRequest(const QUrl &url = QUrl());
};

#endif

