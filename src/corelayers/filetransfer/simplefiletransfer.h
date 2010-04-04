#ifndef SIMPLEFILETRANSFER_H
#define SIMPLEFILETRANSFER_H

#include "libqutim/filetransfer.h"

using namespace qutim_sdk_0_3;
namespace Core
{
class SimpleFileTransfer : public FileTransferManager
{
	Q_OBJECT
public:
	explicit SimpleFileTransfer();
	virtual void send(ChatUnit *unit, const QStringList &files);
	virtual void receive(FileTransferEngine *engine);
};
}

#endif // SIMPLEFILETRANSFER_H
