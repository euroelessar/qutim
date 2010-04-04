#include "simplefiletransfer.h"
#include "filetransferdialog.h"

namespace Core
{
SimpleFileTransfer::SimpleFileTransfer()
{
}

void SimpleFileTransfer::send(ChatUnit *unit, const QStringList &files)
{
	FileTransferEngine *engine = getEngine(unit);
	if (!engine)
		return;
	engine->setFiles(files);
	engine->start();
	receive(engine);
}

void SimpleFileTransfer::receive(FileTransferEngine *engine)
{
	FileTransferDialog *dialog = new FileTransferDialog(engine);
	dialog->show();
}
}
