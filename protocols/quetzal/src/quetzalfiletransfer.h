//#ifndef QUETZALFILETRANSFER_H
//#define QUETZALFILETRANSFER_H
//
//#include <purple.h>
//#include <qutim/filetransfer.h>
//#include "quetzalcontact.h"
//
//using namespace qutim_sdk_0_3;
//class QuetzalFileTransfer;
//
//class QuetzalFileTransferEngine : public FileTransferEngine
//{
//	Q_OBJECT
//public:
//	QuetzalFileTransferEngine(QuetzalContact *, Direction, QuetzalFileTransfer *);
//	virtual int progress() const;
//	virtual int currentFile() const;
//	virtual QStringList files() const;
//	virtual QStringList remoteFiles() const;
//	virtual void setFiles(const QStringList &);
//	virtual qint64 totalSize() const;
//	virtual qint64 fileSize() const;
//	virtual int localPort() const;
//	virtual int remotePort() const;
//	virtual QHostAddress remoteAddress() const;
//	Direction direction() const;
//	virtual State state() const;
//	virtual void start();
//	virtual void cancel();
//};
//
//class QuetzalFileTransfer : public FileTransferFactory
//{
//	Q_OBJECT
//public:
//    QuetzalFileTransfer();
//	virtual bool check(ChatUnit *unit);
//	virtual FileTransferEngine *create(ChatUnit *unit);
//};
//
//extern PurpleXferUiOps quetzal_xfer_uiops;
//
//#endif // QUETZALFILETRANSFER_H
