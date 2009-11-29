/*
    yandexnarodManage

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

#ifndef YANDEXNARODMANAGE_H
#define YANDEXNARODMANAGE_H

#include <QWidget>
#include <QSettings>
#include <QClipboard>
#include "ui_yandexnarodmanage.h"

#include "yandexnarodnetman.h"
#include "uploaddialog.h"

class yandexnarodManage : public QWidget, public Ui::yandexnarodManageClass
{
Q_OBJECT

public:
	yandexnarodManage(QString);
	~yandexnarodManage();
private:
	QString m_profile_name;
	yandexnarodNetMan *netman;
	yandexnarodNetMan *upnetman;
	uploadDialog* uploadwidget;
	QStringList cooks;
	QList<QIcon> fileicons;
	QHash<QString, int> fileiconstyles;
	void netmanPrepare();

struct FileItem
{
	QString fileicon;
	QString fileid;
	QString filename;
	QString fileurl;
};

	QList<FileItem> fileitems;

public slots:
	void setCookies(QStringList incooks) { cooks = incooks; }

private slots:
	void setCooks(QStringList /*incooks*/) { /*if (incooks.count()>0) { cooks = incooks; emit cookies(incooks); }*/ }
	void newFileItem(FileItem);
	void on_btnDelete_clicked();
	void on_btnClipboard_clicked();
	void on_listWidget_pressed(QModelIndex index);
	void on_btnReload_clicked();
	void on_btnUpload_clicked();
	void removeUploadWidget();
	void netmanFinished();

signals:
	void cookies(QStringList);

};
#endif

