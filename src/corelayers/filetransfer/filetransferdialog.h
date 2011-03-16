/****************************************************************************
 *  filetransferdialog.h
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

#ifndef FILETRANSFERDIALOG_H
#define FILETRANSFERDIALOG_H

#include <QDialog>
#include <QFileInfo>
#include <QAbstractItemModel>
#include <qutim/filetransfer.h>

namespace Ui {
    class FileTransferDialog;
}

namespace Core
{
using namespace qutim_sdk_0_3;
class SimpleFileTransfer;

class FileTransferDialog : public QDialog
{
    Q_OBJECT
public:
	FileTransferDialog(QAbstractItemModel *model, SimpleFileTransfer *manager);
    ~FileTransferDialog();
	void setModel(QAbstractItemModel *model);
	int currentJob();
protected:
    void changeEvent(QEvent *e);
private slots:
private:
    Ui::FileTransferDialog *ui;
};

}

#endif // FILETRANSFERDIALOG_H
