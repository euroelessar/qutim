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
	YandexNarodUploadDialog* m_uploadWidget;
	QPointer<YandexNarodManager> m_manageDialog;
	YandexNarodNetMan *m_netMan;
	YandexNarodNetMan *testnetman;
	QString msgtemplate;
	QString purl;
	QTime time;
	QFileInfo fi;
	QStringList cooks;
	bool authtest;

private slots:
	void onActionClicked();
	void onManageClicked();
	void on_btnTest_clicked();
	void on_TestFinished();
	void actionStart();
	void setCooks(QStringList cs) { cooks = cs; }
	void onFileURL(const QString &);

};
#endif
