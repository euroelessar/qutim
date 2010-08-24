#include "simplefiletransfer.h"
#include "filetransferdialog.h"
#include <qutim/actiongenerator.h>
#include <qutim/menucontroller.h>
#include <qutim/icon.h>
#include <qutim/buddy.h>
#include <QFileDialog>

namespace Core
{
	class FileTransferActionGenerator : public ActionGenerator
	{
	public:
		FileTransferActionGenerator(QObject *receiver) :
				ActionGenerator(Icon("document-save"), QT_TRANSLATE_NOOP("FileTransfer", "Send file"),
								receiver, SLOT(onSendFile(QObject*)))
		{
			setType(ActionTypeChatButton|ActionTypeContactList);
			addHandler(ActionVisibilityChangedHandler,receiver);
		}
	};

	SimpleFileTransfer::SimpleFileTransfer()
	{
		static bool init = false;
		if (!init) {
			MenuController::addAction<ChatUnit>(new FileTransferActionGenerator(this));
			init = true;
		}
	}

	void SimpleFileTransfer::send(ChatUnit *unit, const QStringList &files)
	{
		FileTransferEngine *engine = getEngine(unit);
		if (!engine)
			return;
		engine->setFiles(files);
		engine->start();
		FileTransferDialog *dialog = new FileTransferDialog(engine);
#ifdef QUTIM_MOBILE_UI
		dialog->showMaximized();
#else
		dialog->show();
#endif
	}

	void SimpleFileTransfer::receive(FileTransferEngine *engine)
	{
		QString path;
		if (engine->remoteFiles().count() == 1) {
			path = QFileDialog::getSaveFileName(0, QString(), QDir::homePath() + "/" + engine->remoteFiles().first());
		} else {
			path = QFileDialog::getExistingDirectory(0, QString(), QDir::homePath());
		}
		if (!path.isEmpty()) {
			FileTransferDialog *dialog = new FileTransferDialog(engine);
			dialog->setPath(path);
			dialog->show();
		} else {
			engine->cancel();
		}
	}

	void SimpleFileTransfer::onSendFile(QObject *controller)
	{
		ChatUnit *unit = qobject_cast<ChatUnit*>(controller);
		Q_ASSERT(unit);
		QStringList files = QFileDialog::getOpenFileNames(0, QString(), QDir::homePath());
		if (!files.isEmpty()) {
			send(unit, files);
		}
	}

	bool SimpleFileTransfer::event(QEvent *ev)
	{
		if (ev->type() == ActionVisibilityChangedEvent::eventType()) {
			ActionVisibilityChangedEvent *event = static_cast<ActionVisibilityChangedEvent*>(ev);
			if (event->isVisible()) {
				ChatUnit *buddy = qobject_cast<ChatUnit*>(event->controller());
				event->action()->setEnabled(buddy && FileTransferManager::instance()->checkAbility(buddy));
				return true;
			}
		}
		return QObject::event(ev);
	}

}
