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
#include "filetransferjobdelegate.h"
#include "simplefiletransfer.h"
#include "ui_filetransferdialog.h"
#include <QFileDialog>
#include <qutim/actionbox.h>

namespace Core
{

FileTransferDialog::FileTransferDialog(QAbstractItemModel *model, SimpleFileTransfer *manager) :
    ui(new Ui::FileTransferDialog)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);
	setModel(model);
	ui->jobsView->setItemDelegate(new FileTransferJobDelegate(this));

	ActionBox *actions = new ActionBox(this);
	{
		QAction *action = new QAction(QObject::tr("Stop"), actions);
		QObject::connect(action, SIGNAL(triggered()), manager, SLOT(stopCurrentAction()));
		actions->addAction(action);
	}
	{
		QAction *action = new QAction(QObject::tr("Remove"), actions);
		QObject::connect(action, SIGNAL(triggered()), manager, SLOT(removeCurrentAction()));
		actions->addAction(action);
	}
	{
		QAction *action = new QAction(QObject::tr("Close"), actions);
		QObject::connect(action, SIGNAL(triggered()), manager, SLOT(closeFileTransferDialog()));
		actions->addAction(action);
	}
	ui->verticalLayout->addWidget(actions);
}

FileTransferDialog::~FileTransferDialog()
{
	delete ui;
}

void FileTransferDialog::setModel(QAbstractItemModel *model)
{
	ui->jobsView->setModel(model);
}

int FileTransferDialog::currentJob()
{
	return ui->jobsView->currentIndex().row();
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
