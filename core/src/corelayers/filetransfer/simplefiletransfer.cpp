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

template <class T>
T *getObjectFromProperty(QObject *obj, const char *property)
{
	QObject *q_obj = obj->property(property).value<QObject*>();
	T *cast_obj = qobject_cast<T*>(q_obj);
	return cast_obj;
}

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

	QMenu *menu = new QMenu();
	QObject::connect(action, SIGNAL(destroyed()), menu, SLOT(deleteLater()));
	action->setProperty("menuObject", qVariantFromValue((QObject*)menu));
}

static const QList<QAction*> &getSendActions(SimpleFileTransfer *manager)
{
	static QList<QAction*> actions;
	static bool inited = false;
	if (inited)
		return actions;
	inited = true;

	foreach (FileTransferFactory *factory, FileTransferManager::factories()) {
		QAction *action = new QAction(factory->icon(), factory->name(), manager);
		QObject::connect(action, SIGNAL(triggered()), manager, SLOT(onSendThroughSpecificFactory()));
		action->setProperty("factory", qVariantFromValue((QObject*)factory));
		actions << action;
	}
	return actions;
}

static QAction* getDefaultSendAction(SimpleFileTransfer *manager)
{
	static QAction *action = 0;
	if (!action) {
		action = new QAction(QObject::tr("Auto"), manager);
		QObject::connect(action, SIGNAL(triggered()), manager, SLOT(onSendThroughSpecificFactory()));
	}
	return action;
}

static QAction* getSeparatorSendAction(SimpleFileTransfer *manager)
{
	static QAction *action = 0;
	if (!action) {
		action = new QAction(manager);
		action->setSeparator(true);
	}
	return action;
}

void FileTransferActionGenerator::showImpl(QAction *action, QObject *obj)
{
	QMenu *menu = getObjectFromProperty<QMenu>(action, "menuObject");
	ChatUnit *unit = qobject_cast<ChatUnit*>(obj);
	Q_ASSERT(unit && menu);

	foreach (QAction *action, getSendActions(m_manager)) {
		FileTransferFactory *factory = getObjectFromProperty<FileTransferFactory>(action, "factory");
		Q_ASSERT(factory);
		if (factory->checkAbility(unit)) {
			action->setProperty("controller", qVariantFromValue((QObject*)unit));
			menu->addAction(action);
		} else {
			menu->removeAction(action);
		}
	}

	if (menu->actions().count() > 1) {
		QAction *separator = getSeparatorSendAction(m_manager);
		QAction *defaultAct = getDefaultSendAction(m_manager);
		defaultAct->setProperty("controller", qVariantFromValue((QObject*)unit));
		menu->insertAction(menu->actions().first(), separator);
		menu->insertAction(separator, defaultAct);
		action->setMenu(menu);
	} else {
		action->setMenu(0);
	}
}

SimpleFileTransfer::SimpleFileTransfer() :
	m_model(new FileTransferJobModel(this))
{
	m_sendFileActionGen = new FileTransferActionGenerator(this);
	MenuController::addAction<ChatUnit>(m_sendFileActionGen);

	MenuController *contactList = ServiceManager::getByName<MenuController*>("ContactList");
	if (contactList) {
		static ActionGenerator gen(Icon("download-tranfermanager"),
								   QT_TRANSLATE_NOOP("FileTransfer", "Manage file transfers"),
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

void SimpleFileTransfer::confirmDownloading(FileTransferJob *job)
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
											  QDir::home().filePath(job->title()));
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
		confirmDownloading(job);
}

void SimpleFileTransfer::sendFile(ChatUnit *unit, FileTransferFactory *factory)
{
	QString file = QFileDialog::getOpenFileName(
			0,
			tr("Choose file for %1").arg(unit->title()),
			QDir::homePath());
	if (!file.isEmpty()) {
		QUrl url = QUrl::fromLocalFile(file);
		send(unit, url, QString(), factory);
	}
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
	sendFile(unit);
}

void SimpleFileTransfer::onUnitTrasferAbilityChanged(bool ability)
{
	FileTransferObserver *observer = qobject_cast<FileTransferObserver*>(sender());
	Q_ASSERT(observer);
	foreach (QAction *action, m_sendFileActionGen->actions(observer->chatUnit()))
		action->setEnabled(ability);
}

void SimpleFileTransfer::onSendThroughSpecificFactory()
{
	ChatUnit *unit = getObjectFromProperty<ChatUnit>(sender(), "controller");
	FileTransferFactory *factory = getObjectFromProperty<FileTransferFactory>(sender(), "factory");
	Q_ASSERT(unit);
	sendFile(unit, factory);
}

bool SimpleFileTransfer::event(QEvent *ev)
{
	return FileTransferManager::event(ev);
}

}
