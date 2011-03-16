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

FileTransferJob *SimpleFileTransfer::getCurrentJob()
{
	Q_ASSERT(m_dialog);
	int row = m_dialog->currentJob();
	if (row == -1)
		return 0;
	return m_model->getJob(row);
}

void SimpleFileTransfer::openFileTransferDialog()
{
	if (!m_dialog)
		m_dialog = new FileTransferDialog(m_model, this);
	SystemIntegration::show(m_dialog.data());
}

void SimpleFileTransfer::closeFileTransferDialog()
{
	m_dialog->deleteLater();
}

void SimpleFileTransfer::stopCurrentAction()
{
	FileTransferJob *job = getCurrentJob();
	if (job)
		job->stop();
}

void SimpleFileTransfer::removeCurrentAction()
{
	FileTransferJob *job = getCurrentJob();
	if (job)
		job->deleteLater();
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
