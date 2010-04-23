#ifndef SIMPLEFILETRANSFER_H
#define SIMPLEFILETRANSFER_H

#include "libqutim/filetransfer.h"

using namespace qutim_sdk_0_3;
namespace Core
{

class SimpleFileTransfer : public FileTransferManager
{
	Q_OBJECT
	Q_CLASSINFO("Service", "FileTransferManager")
	Q_CLASSINFO("Uses", "IconLoader")
public:
	explicit SimpleFileTransfer();
	virtual void send(ChatUnit *unit, const QStringList &files);
	virtual void receive(FileTransferEngine *engine);
private slots:
	void onSendFile();
};

}

#endif // SIMPLEFILETRANSFER_H
