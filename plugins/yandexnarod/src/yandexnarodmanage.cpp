/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Alexander Kazarin <boiler@co.ru>
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

#include <qutim/configbase.h>
#include "yandexnarodmanage.h"

using namespace qutim_sdk_0_3;

YandexNarodManager::YandexNarodManager()
{
	setupUi(this);
	this->setWindowTitle(tr("Yandex.Narod file manager"));
	this->setWindowIcon(QIcon(":/icons/yandexnarodplugin.png"));
	frameProgress->hide();
	frameFileActions->hide();
	listWidget->clear();

	netman = new YandexNarodNetMan(this);
	connect(netman, SIGNAL(statusText(QString)), labelStatus, SLOT(setText(QString)));
	connect(netman, SIGNAL(progressMax(int)), progressBar, SLOT(setMaximum(int)));
	connect(netman, SIGNAL(progressValue(int)), progressBar, SLOT(setValue(int)));
	connect(netman, SIGNAL(newFileItem(FileItem)), this, SLOT(newFileItem(FileItem)));
	connect(netman, SIGNAL(finished()), this, SLOT(netmanFinished()));

	QPixmap iconimage(":/icons/yandexnarod-icons-files.png");
	for (int i=0; i<(iconimage.width()/16); i++) {
		QIcon icon(iconimage.copy((i*16),0,16,16));
		fileicons.append(icon);
	}

	fileiconstyles["b-icon-music"] = 0;
	fileiconstyles["b-icon-video"] = 1;
	fileiconstyles["b-icon-arc"] = 2;
	fileiconstyles["b-icon-doc"] = 3;
	fileiconstyles["b-icon-soft"] = 4;
	fileiconstyles["b-icon-unknown"] = 5;
	fileiconstyles["b-icon-picture"] = 14;

	uploadwidget=0;

	ConfigGroup group = Config().group("yandex").group("narod");

	QByteArray geometry = group.value("managerGeometry", QByteArray());
	if (!geometry.isEmpty())
		restoreGeometry(geometry);
	else
		centerizeWidget(this);

	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

YandexNarodManager::~YandexNarodManager()
{
	delete netman;
	ConfigGroup group = Config().group("yandex").group("narod");
	group.setValue("managerGeometry", saveGeometry());
	group.sync();
}

void YandexNarodManager::newFileItem(FileItem fileitem)
{
	int iconnum = 5;
	QString fileiconname = fileitem.fileicon.replace("-old", "");
	if (fileiconstyles.contains(fileiconname)) iconnum = fileiconstyles[fileiconname];

	QListWidgetItem *listitem = new QListWidgetItem(fileicons[iconnum], fileitem.filename);
	listWidget->addItem(listitem);

	fileitems.append(fileitem);
}

void YandexNarodManager::netmanPrepare()
{
	progressBar->setValue(0);
	frameProgress->show();
	labelStatus->clear();
	frameFileActions->hide();
	btnReload->setEnabled(false);
}

void YandexNarodManager::netmanFinished()
{
	btnReload->setEnabled(true);
}

void YandexNarodManager::on_btnReload_clicked()
{
	listWidget->clear();
	fileitems.clear();

	netmanPrepare();
	netman->startGetFilelist();
}

void YandexNarodManager::on_btnDelete_clicked()
{
	progressBar->setMaximum(1);
	netmanPrepare();

	QStringList delfileids;
	for (int i=0; i<listWidget->count(); i++) {
		if (listWidget->item(i)->isSelected()) {
			listWidget->item(i)->setIcon(fileicons[15]);
			delfileids.append(fileitems[i].fileid);
		}
	}

	netman->startDelFiles(delfileids);
}

void YandexNarodManager::on_listWidget_pressed(QModelIndex)
{
	if (progressBar->value()==progressBar->maximum()) frameProgress->hide();
	if (frameFileActions->isHidden()) frameFileActions->show();
}

void YandexNarodManager::on_btnClipboard_clicked()
{
	QString text;
	for (int i=0; i<listWidget->count(); i++) {
		if (listWidget->item(i)->isSelected()) {
			text += fileitems[i].fileurl+"\n";
		}
	}
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(text);
}


void YandexNarodManager::on_btnUpload_clicked()
{
//	new YandexNarodUploadDialog(m_nM)
//		uploadwidget = new YandexNarodUploadDialog();
//		connect(uploadwidget, SIGNAL(canceled()), this, SLOT(removeUploadWidget()));
//		uploadwidget->show();
//
//		ConfigGroup group = Config().group("yandexnarod");
//		QString filepath = QFileDialog::getOpenFileName(uploadwidget,
//														tr("Choose file"),
//														group.value("lastdir", QString()));
//
//		if (filepath.length()>0) {
//			QFileInfo fi(filepath);
//			group.setValue("lastdir", fi.dir().path());
//			group.sync();
//			upnetman = new YandexNarodNetMan(uploadwidget);
//			connect(upnetman, SIGNAL(statusText(QString)), uploadwidget, SLOT(setStatus(QString)));
//			connect(upnetman, SIGNAL(statusFileName(QString)), uploadwidget, SLOT(setFilename(QString)));
//			connect(upnetman, SIGNAL(transferProgress(qint64,qint64)), uploadwidget, SLOT(progress(qint64,qint64)));
//			connect(upnetman, SIGNAL(uploadFinished()), uploadwidget, SLOT(setDone()));
//			connect(upnetman, SIGNAL(finished()), this, SLOT(netmanFinished()));
//			upnetman->startUploadFile(filepath);
//		}
//		else {
//			delete uploadwidget; uploadwidget=0;
//		}


}

void YandexNarodManager::removeUploadWidget()
{

}

