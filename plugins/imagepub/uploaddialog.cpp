/*
    uploadDialog

		Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "uploaddialog.h"

uploadDialog::uploadDialog( )
{
	ui.setupUi(this);

	utime.start();

	connect(ui.btnUploadCancel, SIGNAL(clicked()), this, SIGNAL(canceled()));
	move(desktop.width()/2 - width()/2, desktop.height()/2 - height()/2);

	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

uploadDialog::~uploadDialog() 
{
}

void uploadDialog::timeStart() {
	ui.progressBar->setValue(0);
	utime.start();
}

void uploadDialog::progress(qint64 cBytes, qint64 totalBytes) {
	ui.labelStatus->setText(tr("Uploading..."));
	ui.labelProgress->setText(QString(tr("Progress: %1 / %2")).arg(QString::number(cBytes)).arg(QString::number(totalBytes)));
	ui.progressBar->setMaximum(totalBytes);
	ui.progressBar->setValue(cBytes);
	
	QTime etime(0,0,0);
	etime = etime.addMSecs(utime.elapsed());
	ui.labelETime->setText(QString(tr("Elapsed time: %1")).arg(etime.toString("hh:mm:ss")));
	
	if (utime.elapsed()>0) {
		float speed_kbsec = (cBytes / (utime.elapsed()));
		if (speed_kbsec>0) ui.labelSpeed->setText(QString(tr("Speed: %1 kb/sec")).arg(QString::number(speed_kbsec)));
	}
}
