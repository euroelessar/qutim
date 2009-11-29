/*
    uploadDialog

    Copyright (c) 2008 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef UPLOADDIALOG_H
#define UPLOADDIALOG_H

#include <QTime>
#include "ui_uploaddialog.h"

class uploadDialog : public QWidget  
{
	Q_OBJECT;

public:
	uploadDialog();
	~uploadDialog();
	void start();

private:
	Ui::uploadDialogClass ui;
	QTime utime;

signals:
	void canceled();

public slots:
	void progress(qint64, qint64);
	void setStatus(QString str) { ui.labelStatus->setText(str); }
	void setFilename(QString str) { ui.labelFile->setText("File: "+str); this->setWindowTitle(tr("Uploading")+" - "+str); }
	void setDone() { ui.btnUploadCancel->setText(tr("Done")); }

};
#endif
