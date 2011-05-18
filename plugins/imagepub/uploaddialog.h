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

#ifndef UPLOADDIALOG_H
#define UPLOADDIALOG_H

#include <QTime>
#include <QDesktopWidget>
#include "ui_uploaddialog.h"

class uploadDialog : public QWidget  
{
	Q_OBJECT;

public:
	uploadDialog();
	~uploadDialog();
	void setStatus(QString str) { ui.labelStatus->setText(str); }
	void setFilename(QString str) { ui.labelFile->setText(QString(tr("File: %1")).arg(str)); }
	void timeStart();

private:
	Ui::uploadDialogClass ui;
	QDesktopWidget desktop;
	QTime utime;

signals:
	void canceled();

public slots:
	void progress(qint64, qint64);

};
#endif
