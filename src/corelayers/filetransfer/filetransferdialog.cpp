/****************************************************************************
 *  filetransferdialog.cpp
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "filetransferdialog.h"
#include "ui_filetransferdialog.h"
#include <QFileDialog>

namespace Core
{
FileTransferDialog::FileTransferDialog(QAbstractItemModel *model) :
    ui(new Ui::FileTransferDialog)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);
	setModel(model);
}

FileTransferDialog::~FileTransferDialog()
{
	delete ui;
}

void FileTransferDialog::setModel(QAbstractItemModel *model)
{
	ui->jobsView->setModel(model);
}

void FileTransferDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

}
