/****************************************************************************
 *  yandexnarod.h
 *
 *  Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>
 *                     2010 by Nigmatullin Ruslan <euroelessar@ya.ru>
 *                     2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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
	void onActionClicked(QObject*);
	void onManageClicked();
	void on_btnTest_clicked();
	void on_TestFinished();
	void actionStart();
//	void setCooks(QStringList cs) { cooks = cs; }
	void onFileURL(const QString &);
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
