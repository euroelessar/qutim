/****************************************************************************
 *  uploaddialog.cpp
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

#include <qutim/libqutim_global.h>
#include "uploaddialog.h"

YandexNarodUploadDialog::YandexNarodUploadDialog( )
{
	ui.setupUi(this);
	utime.start();
	connect(ui.btnUploadCancel, SIGNAL(clicked()), this, SIGNAL(canceled()));
	connect(ui.btnUploadCancel, SIGNAL(clicked()), this, SLOT(close()));
	qutim_sdk_0_3::centerizeWidget(this);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

YandexNarodUploadDialog::~YandexNarodUploadDialog()
{
	
}

void YandexNarodUploadDialog::start() {
	ui.progressBar->setValue(0);
	utime.start();
}

void YandexNarodUploadDialog::progress(qint64 cBytes, qint64 totalBytes) {
	ui.labelStatus->setText("Uploading...");
	ui.labelProgress->setText("Progress: "+QString::number(cBytes)+" / "+QString::number(totalBytes));
	ui.progressBar->setMaximum(totalBytes);
	ui.progressBar->setValue(cBytes);

	setWindowTitle("[" + ui.progressBar->text() + "] - Uploading...");
	
	QTime etime(0,0,0);
	etime = etime.addMSecs(utime.elapsed());
	ui.labelETime->setText("Elapsed time: " + etime.toString("hh:mm:ss") );
	
	float speed_kbsec = (cBytes / (utime.elapsed()/1000))/1024;
	if (speed_kbsec>0) ui.labelSpeed->setText("Speed: "+QString::number(speed_kbsec)+" kb/sec");
	
	if (cBytes == totalBytes) ui.labelStatus->setText("Upload complete.");
}
