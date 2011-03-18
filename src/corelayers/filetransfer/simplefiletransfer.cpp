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
#include <qutim/menucontroller.h>
#include <qutim/icon.h>
#include <qutim/buddy.h>
#include <qutim/systemintegration.h>
#include <qutim/servicemanager.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QUrl>

namespace Core
{

FileTransferActionGenerator::FileTransferActionGenerator(SimpleFileTransfer *manager) :
	ActionGenerator(Icon("document-save"),
					QT_TRANSLATE_NOOP("FileTransfer", "Send file"),
					manager,
					SLOT(onSendFile(QObject*))),
	m_manager(manager)
{
	setType(ActionTypeChatButton | ActionTypeContactList);
}

void FileTransferActionGenerator::createImpl(QAction *action, QObject *obj) const
{
	ChatUnit *unit = qobject_cast<ChatUnit*>(obj);
	if (!unit)
		return;
	FileTransferObserver *observer = new FileTransferObserver(unit);
	QObject::connect(action, SIGNAL(destroyed()),
					 observer, SLOT(deleteLater()));
	QObject::connect(observer, SIGNAL(abilityChanged(bool)),
					 m_manager, SLOT(onUnitTrasferAbilityChanged(bool)));
	action->setEnabled(observer->checkAbility());
}

SimpleFileTransfer::SimpleFileTransfer() :
	m_model(new FileTransferJobModel(this))
{
	m_sendFileActionGen = new FileTransferActionGenerator(this);
	MenuController::addAction<ChatUnit>(m_sendFileActionGen);

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
		path = job->property("localPath").toString();

		if (path.isNull())
			return 0;

		QFileInfo info = path;
		if (!info.isDir())
			path = info.absoluteFilePath();
		else
			path = QDir(path).filePath(job->fileName());
	} else {
		path = job->baseDir().filePath(job->fileName());
	}
	return new QFile(path);
}

void SimpleFileTransfer::doConfirmDownloading(FileTransferJob *job)
{
	QString path = job->property("localPath").toString();
	if (!path.isEmpty())
		return;

	int result = QMessageBox::question(
			m_dialog.data(),
			tr("Incoming file"),
			tr("%1 wants to send you %2 (%3)")
				.arg(job->chatUnit()->title())
				.arg(job->title())
				.arg(bytesToString(job->totalSize())),
			tr("Accept"),
			tr("Ignore"));
	if (result != 0)
		return;

	if (job->filesCount() == 1) {
		path = QFileDialog::getSaveFileName(0, QString(),
											  QDir::home().filePath(job->fileName()));
	} else {
		path = QFileDialog::getExistingDirectory(0, QString(), QDir::homePath());
	}

	if (path.isEmpty()) {
		job->stop();
	} else {
		job->setProperty("localPath", path);
		job->accept();
	}
}

void SimpleFileTransfer::handleJob(FileTransferJob *job, FileTransferJob *oldJob)
{
	if (oldJob)
		job->setProperty("localPath", oldJob->property("localPath"));
	m_model->handleJob(job, oldJob);
	openFileTransferDialog();
	if (job->direction() == FileTransferJob::Incoming)
		doConfirmDownloading(job);
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

void SimpleFileTransfer::onUnitTrasferAbilityChanged(bool ability)
{
	FileTransferObserver *observer = qobject_cast<FileTransferObserver*>(sender());
	Q_ASSERT(observer);
	foreach (QAction *action, m_sendFileActionGen->actions(observer->chatUnit()))
		action->setEnabled(ability);
}

bool SimpleFileTransfer::event(QEvent *ev)
{
	return FileTransferManager::event(ev);
}

}
