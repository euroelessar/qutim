/****************************************************************************
 *  yandexnarod.h
 *
 *  Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>
 *                     2010 by Nigmatullin Ruslan <euroelessar@ya.ru>
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
#include "uploaddialog.h"

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

	QNetworkAccessManager *m_networkManager;
	YandexNarodAuthorizator *m_authorizator;

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
#endif
