/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
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

#ifndef YANDEXNARODMANAGE_H
#define YANDEXNARODMANAGE_H

#include <QWidget>
#include <QClipboard>
#include "ui_yandexnarodmanage.h"

#include "yandexnarodnetman.h"
#include "yandexnaroduploadjob.h"

class YandexNarodManager : public QWidget, public Ui::yandexnarodManageClass
{
Q_OBJECT

public:
	YandexNarodManager();
	~YandexNarodManager();

private:
	void netmanPrepare();

	YandexNarodNetMan *netman;
	YandexNarodNetMan *upnetman;
	YandexNarodUploadJob* uploadwidget;
	QStringList cooks;
	QList<QIcon> fileicons;
	QHash<QString, int> fileiconstyles;

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

