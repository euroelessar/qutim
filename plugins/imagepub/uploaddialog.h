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

