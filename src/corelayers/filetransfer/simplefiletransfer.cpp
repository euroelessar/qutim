#include "simplefiletransfer.h"
#include <qutim/actiongenerator.h>
#include <qutim/menucontroller.h>
#include <qutim/icon.h>
#include <qutim/buddy.h>
#include <qutim/systemintegration.h>
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

SimpleFileTransfer::SimpleFileTransfer()
{
	MenuController::addAction<ChatUnit>(new FileTransferActionGenerator(this));
}

QIODevice *SimpleFileTransfer::doOpenFile(FileTransferJob *job)
{
	FileTransferDialog *dialog = m_jobs.value(job);
	if (dialog)
		return dialog->openFile();
	return 0;
}

void SimpleFileTransfer::handleJob(FileTransferJob *job, FileTransferJob *oldJob)
{
	FileTransferDialog *dialog = m_jobs.take(oldJob);
	if (dialog)
		dialog->setJob(job);
	else
		dialog = new FileTransferDialog(job);
	SystemIntegration::show(dialog);
	m_jobs.insert(job, dialog);
}

//	void SimpleFileTransfer::send(ChatUnit *unit, const QStringList &files)
//	{
//		FileTransferEngine *engine = getEngine(unit);
//		if (!engine)
//			return;
//		engine->setFiles(files);
//		engine->start();
//		FileTransferDialog *dialog = new FileTransferDialog(engine);
//#ifdef QUTIM_MOBILE_UI
//		dialog->showMaximized();
//#else
//		dialog->show();
//#endif
//	}

//	void SimpleFileTransfer::receive(FileTransferEngine *engine)
//	{
//		QString path;
//		if (engine->remoteFiles().count() == 1) {
//			path = QFileDialog::getSaveFileName(0, QString(), QDir::homePath() + "/" + engine->remoteFiles().first());
//		} else {
//			path = QFileDialog::getExistingDirectory(0, QString(), QDir::homePath());
//		}
//		if (!path.isEmpty()) {
//			FileTransferDialog *dialog = new FileTransferDialog(engine);
//			dialog->setPath(path);
//			dialog->show();
//		} else {
//			engine->cancel();
//		}
//	}

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

void SimpleFileTransfer::onDialogDeath(QObject *obj)
{
	FileTransferDialog *dialog = static_cast<FileTransferDialog*>(obj);
	m_jobs.remove(m_jobs.key(dialog));
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
