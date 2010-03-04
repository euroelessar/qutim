#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include "libqutim_global.h"
#include <QHostAddress>

namespace qutim_sdk_0_3
{
class FileTransferEngine : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
	Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
	Q_PROPERTY(QString remoteFileName READ remoteFileName NOTIFY remoteFileNameChanged)
	Q_PROPERTY(qint64 fileSize READ fileSize NOTIFY fileSizeChanged)
	Q_PROPERTY(int localPort READ localPort NOTIFY localPortChanged)
	Q_PROPERTY(int remotePort READ remotePort NOTIFY remotePortChanged)
	Q_PROPERTY(QHostAddress remoteAddress READ remoteAddress NOTIFY remoteAddressChanged)
	Q_PROPERTY(Direction direction READ direction CONST)
	Q_PROPERTY(State state READ state NOTIFY stateChanged)
public:
	enum Direction { Send, Receive };
	enum State
	{
		StateUnknown = 0,   /**< Unknown, the xfer may be null. */
		StateNotStarted,    /**< It hasn't started yet. */
		StateAccepted,      /**< Receive accepted, but destination file not selected yet */
		StateStarted,       /**< purple_xfer_start has been called. */
		StateDone,          /**< The xfer completed successfully. */
		StateCancelLocal,   /**< The xfer was canceled by us. */
		StateCancelRemote   /**< The xfer was canceled by the other end, or we couldn't connect. */
	};
	Q_ENUMS(Direction)
	Q_ENUMS(State)
	explicit FileTransferEngine();
	virtual int progress() const = 0;
	virtual QString fileName() const = 0;
	virtual void setFileName(const QString &) = 0;
	virtual qint64 fileSize() const = 0;
	virtual int localPort() const = 0;
	virtual int remotePort() const = 0;
	virtual QHostAddress remoteAddress() const = 0;
	virtual Direction direction() const = 0;
	virtual State state() const = 0;
public slots:
	virtual void start() = 0;
	virtual void cancel() = 0;

protected:
	virtual void virtual_hook(int id, void *data);

signals:
	// From 0 to 100
	void progressChanged(int percent);
	void remoteAddressChanged(const QHostAddress &);
	void fileNameChanged(const QString &);
	void remoteFileNameChanged(const QString &);
	void fileSizeChanged(qint64);
	void localPortChanged(int);
	void remotePortChanged(int);
	void remoteAddressChanged(int);
	void stateChanged(qutim_sdk_0_3::FileTransferEngine::State);
};

class FileTransfer : public QObject
{
	Q_OBJECT
public:
	explicit FileTransfer();
	virtual void setEngine(FileTransferEngine *) = 0;
signals:

public slots:
};
}

#endif // FILETRANSFER_H
