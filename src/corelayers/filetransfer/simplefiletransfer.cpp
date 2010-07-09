#include "simplefiletransfer.h"
#include "filetransferdialog.h"
#include "src/modulemanagerimpl.h"
#include "libqutim/actiongenerator.h"
#include "libqutim/menucontroller.h"
#include "libqutim/icon.h"
#include "libqutim/buddy.h"
#include <QFileDialog>

namespace Core
{

static Core::CoreModuleHelper<SimpleFileTransfer> filetransfer_static(
	QT_TRANSLATE_NOOP("Plugin", "File transfer manager"),
	QT_TRANSLATE_NOOP("Plugin", "Default qutIM file transfer manager")
);

class FileTransferActionGenerator : public ActionGenerator
{
public:
	FileTransferActionGenerator(QObject *receiver) :
			ActionGenerator(Icon("document-save"), QT_TRANSLATE_NOOP("FileTransfer", "Send file"),
							receiver, SLOT(onSendFile(QObject*))) {}
protected:
    virtual void showImpl(QAction *action, QObject *obj)
	{
		ChatUnit *buddy = qobject_cast<ChatUnit*>(obj);
		action->setVisible(buddy && FileTransferManager::instance()->checkAbility(buddy));
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
	dialog->show();
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

}
