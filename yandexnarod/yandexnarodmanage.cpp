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

#include <qutim/plugininterface.h>
#include "yandexnarodmanage.h"

yandexnarodManage::yandexnarodManage(QString profile_name)
{
	m_profile_name = profile_name;
	setupUi(this);
	this->setWindowTitle(tr("Yandex.Narod file manager"));
	this->setWindowIcon(QIcon(":/icons/yandexnarodplugin.png"));
	frameProgress->hide();
	frameFileActions->hide();
	listWidget->clear();

	netman = new yandexnarodNetMan(this, m_profile_name);
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

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+profile_name, "plugin_yandexnarod");
	if (settings.value("manager/width").isValid() && settings.value("manager/height").isValid()) {
		this->resize(settings.value("manager/width").toInt(), settings.value("manager/height").toInt());
	}
	if (settings.value("manager/left").isValid() && settings.value("manager/top").isValid()) {
		move(settings.value("manager/left").toInt(), settings.value("manager/top").toInt());
	}
	else {
		qutim_sdk_0_2::SystemsCity::PluginSystem()->centerizeWidget(this);
	}


	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

yandexnarodManage::~yandexnarodManage()
{
	delete netman;
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_yandexnarod");
	settings.setValue("manager/left", this->geometry().left());
	settings.setValue("manager/top", this->geometry().top());
	settings.setValue("manager/width", this->geometry().width());
	settings.setValue("manager/height", this->geometry().height());
}

void yandexnarodManage::newFileItem(FileItem fileitem)
{
	int iconnum = 5;
	QString fileiconname = fileitem.fileicon.replace("-old", "");
	if (fileiconstyles.contains(fileiconname)) iconnum = fileiconstyles[fileiconname];

	QListWidgetItem *listitem = new QListWidgetItem(fileicons[iconnum], fileitem.filename);
	listWidget->addItem(listitem);

	fileitems.append(fileitem);
}

void yandexnarodManage::netmanPrepare()
{
	progressBar->setValue(0);
	frameProgress->show();
	labelStatus->clear();
	frameFileActions->hide();
	btnReload->setEnabled(false);
}

void yandexnarodManage::netmanFinished()
{
	btnReload->setEnabled(true);
}

void yandexnarodManage::on_btnReload_clicked()
{
	listWidget->clear();
	fileitems.clear();

	netmanPrepare();
	netman->startGetFilelist();
}

void yandexnarodManage::on_btnDelete_clicked()
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

void yandexnarodManage::on_listWidget_pressed(QModelIndex)
{
	if (progressBar->value()==progressBar->maximum()) frameProgress->hide();
	if (frameFileActions->isHidden()) frameFileActions->show();
}

void yandexnarodManage::on_btnClipboard_clicked()
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


void yandexnarodManage::on_btnUpload_clicked()
{

		uploadwidget = new uploadDialog();
		connect(uploadwidget, SIGNAL(canceled()), this, SLOT(removeUploadWidget()));
		uploadwidget->show();

		QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_yandexnarod");
		QString filepath = QFileDialog::getOpenFileName(uploadwidget, tr("Choose file"), settings.value("main/lastdir").toString());

		if (filepath.length()>0) {
			QFileInfo fi(filepath);
			settings.setValue("main/lastdir", fi.dir().path());
			upnetman = new yandexnarodNetMan(uploadwidget, m_profile_name);
			connect(upnetman, SIGNAL(statusText(QString)), uploadwidget, SLOT(setStatus(QString)));
			connect(upnetman, SIGNAL(statusFileName(QString)), uploadwidget, SLOT(setFilename(QString)));
			connect(upnetman, SIGNAL(transferProgress(qint64,qint64)), uploadwidget, SLOT(progress(qint64,qint64)));
			connect(upnetman, SIGNAL(uploadFinished()), uploadwidget, SLOT(setDone()));
			connect(upnetman, SIGNAL(finished()), this, SLOT(netmanFinished()));
			upnetman->startUploadFile(filepath);
		}
		else {
			delete uploadwidget; uploadwidget=0;
		}


}

void yandexnarodManage::removeUploadWidget()
{

}
