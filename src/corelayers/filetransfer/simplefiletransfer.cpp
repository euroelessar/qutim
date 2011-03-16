/****************************************************************************
 *  simplefiletransfer.cpp
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

#include "simplefiletransfer.h"
#include <qutim/actiongenerator.h>
#include <qutim/menucontroller.h>
#include <qutim/icon.h>
#include <qutim/buddy.h>
#include <qutim/systemintegration.h>
#include <qutim/servicemanager.h>
#include <QFileDialog>
#include <QUrl>

namespace Core
{
class FileTransferActionGenerator : public ActionGenerator
{
public:
	FileTransferActionGenerator(QObject *receiver) :
		ActionGenerator(Icon("document-save"), QT_TRANSLATE_NOOP("FileTransfer", "Send file"),
			receiver, SLOT(onSendFile(QObject*)))
	{
		setType(ActionTypeChatButton | ActionTypeContactList);
		addHandler(ActionVisibilityChangedHandler, receiver);
	}
};

SimpleFileTransfer::SimpleFileTransfer() :
	m_model(new FileTransferJobModel(this))
{
	MenuController::addAction<ChatUnit>(new FileTransferActionGenerator(this));

	MenuController *contactList = ServiceManager::getByName<MenuController*>("ContactList");
	if (contactList) {
		static ActionGenerator gen(Icon("download-tranfermanager"),
								   QT_TR_NOOP("Manage file transfers"),
								   this,
								   SLOT(openFileTransferDialog()));
		gen.setType(ActionTypeContactList);
		contactList->addAction(&gen);
	}
}

QIODevice *SimpleFileTransfer::doOpenFile(FileTransferJob *job)
{
	if (!m_model->containsJob(job))
		return 0;

	QString path;
	if (job->direction() == FileTransferJob::Incoming) {
		path = m_paths.value(job);
		if (path.isNull()) {
			if (job->filesCount() == 1) {
				path = QFileDialog::getSaveFileName(0, QString(),
													  QDir::home().filePath(job->fileName()));
			} else {
				path = QFileDialog::getExistingDirectory(0, QString(), QDir::homePath());
			}
			if (path.isEmpty()) {
				job->stop();
				return 0;
			}
			m_paths.insert(job, path);
		}

		QFileInfo info = path;
		if (info.isFile())
			path = info.absoluteFilePath();
		else
			path = QDir(path).filePath(job->fileName());
	} else {
		path = job->baseDir().filePath(job->fileName());
	}
	return new QFile(path);
}

void SimpleFileTransfer::handleJob(FileTransferJob *job, FileTransferJob *oldJob)
{
	if (oldJob)
		m_paths.insert(job, m_paths.take(oldJob));
	connect(job, SIGNAL(destroyed(QObject*)), SLOT(onJobDestroyed(QObject*)));
	m_model->handleJob(job, oldJob);
	openFileTransferDialog();
}

void SimpleFileTransfer::openFileTransferDialog()
{
	if (!m_dialog)
		m_dialog = new FileTransferDialog(m_model);
	SystemIntegration::show(m_dialog.data());
}

void SimpleFileTransfer::onSendFile(QObject *controller)
{
	ChatUnit *unit = qobject_cast<ChatUnit*>(controller);
	Q_ASSERT(unit);
	QString file = QFileDialog::getOpenFileName(0, QString(), QDir::homePath());
	if (!file.isEmpty()) {
		QUrl url = QUrl::fromLocalFile(file);
		send(unit, url);
	}
}

void SimpleFileTransfer::onJobDestroyed(QObject *obj)
{
	m_paths.remove(static_cast<FileTransferJob*>(obj));
}

bool SimpleFileTransfer::event(QEvent *ev)
{
	if (ev->type() == ActionVisibilityChangedEvent::eventType()) {
		ActionVisibilityChangedEvent *event = static_cast<ActionVisibilityChangedEvent*>(ev);
		if (event->isVisible()) {
			ChatUnit *buddy = qobject_cast<ChatUnit*>(event->controller());
			event->action()->setEnabled(buddy && checkAbility(buddy));
			return true;
		}
	}
	return FileTransferManager::event(ev);
}

}
