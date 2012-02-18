/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef SIMPLEFILETRANSFER_H
#define SIMPLEFILETRANSFER_H

#include <QPointer>
#include <qutim/filetransfer.h>
#include <qutim/actiongenerator.h>
#include "filetransferdialog.h"
#include "filetransferjobmodel.h"

using namespace qutim_sdk_0_3;
namespace Core
{

class SimpleFileTransfer;

class FileTransferActionGenerator : public ActionGenerator
{
public:
	FileTransferActionGenerator(SimpleFileTransfer *manager);
	void createImpl(QAction *action, QObject *obj) const;
	void showImpl(QAction *action,QObject *obj);

private:
	SimpleFileTransfer *m_manager;
};

class SimpleFileTransfer : public FileTransferManager
{
	Q_OBJECT
	Q_CLASSINFO("Uses", "IconLoader")
public:
	explicit SimpleFileTransfer();
	virtual QIODevice *doOpenFile(FileTransferJob *job);
	virtual void handleJob(FileTransferJob *job, FileTransferJob *oldJob);
	void confirmDownloading(FileTransferJob *job);
	bool event(QEvent *ev);
private:
	void sendFile(ChatUnit *unit, FileTransferFactory *factory = 0);
private slots:
	void openFileTransferDialog();
	void onSendFile(QObject *controller);
	void onUnitTrasferAbilityChanged(bool);
	void onSendThroughSpecificFactory();
private:
	FileTransferJobModel *m_model;
	QPointer<FileTransferDialog> m_dialog;
	ActionGenerator *m_sendFileActionGen;
};

}

#endif // SIMPLEFILETRANSFER_H

