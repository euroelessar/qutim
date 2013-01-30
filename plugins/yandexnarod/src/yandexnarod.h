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
#include <qutim/plugin.h>
#include <QAction>
#include <QFileDialog>

#include "yandexnarodmanage.h"
#include "yandexnarodsettings.h"
#include "yandexnarodauthorizator.h"

using namespace qutim_sdk_0_3;

class YandexNarodPlugin : public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugInfo", "Yandex")
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
private:
	void loadCookies();
private slots:
	void saveCookies();
	void on_btnTest_clicked();
};

class YandexNarodFactory : public FileTransferFactory
{
	Q_OBJECT
public:
	YandexNarodFactory();
	virtual bool checkAbility(ChatUnit *unit);
	virtual bool startObserve(ChatUnit *unit);
	virtual bool stopObserve(ChatUnit *unit);
	virtual FileTransferJob *create(ChatUnit *unit);
	static QNetworkAccessManager *networkManager();
	static YandexNarodAuthorizator *authorizator();
private slots:
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status);
	void onAccountAdded(qutim_sdk_0_3::Account *account);
private:
	typedef QMultiMap<QObject*, ChatUnit*> Observers;
	Observers m_observedUnits;
};

class YandexRequest : public QNetworkRequest
{
public:
	YandexRequest(const QUrl &url = QUrl());
};

#endif

