/****************************************************************************
 *  filetransfer.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include "libqutim_global.h"
#include <QHostAddress>

namespace qutim_sdk_0_3
{
class ChatUnit;
class FileTransferFactory;
class FileTransferManager;
class FileTransferEnginePrivate;
class FileTransferManagerPrivate;

class LIBQUTIM_EXPORT FileTransferEngine : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(FileTransferEngine)
	Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
	Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
	Q_PROPERTY(QString remoteFileName READ remoteFileName NOTIFY remoteFileNameChanged)
	Q_PROPERTY(qint64 fileSize READ fileSize NOTIFY fileSizeChanged)
	Q_PROPERTY(int localPort READ localPort NOTIFY localPortChanged)
	Q_PROPERTY(int remotePort READ remotePort NOTIFY remotePortChanged)
	Q_PROPERTY(QHostAddress remoteAddress READ remoteAddress NOTIFY remoteAddressChanged)
	Q_PROPERTY(Direction direction READ direction CONSTANT)
	Q_PROPERTY(State state READ state NOTIFY stateChanged)
	Q_PROPERTY(ChatUnit* chatUnit READ chatUnit CONSTANT)
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
	enum Error
	{
		ErrorTryNext,
		ErrorTryAgain
	};
	Q_ENUMS(Direction)
	Q_ENUMS(State)
	Q_ENUMS(Error)
	explicit FileTransferEngine(ChatUnit *, FileTransferFactory *);
	virtual ~FileTransferEngine();
	ChatUnit *chatUnit() const;
	virtual int progress() const = 0;
	virtual QString fileName() const = 0;
	virtual QString remoteFileName() const = 0;
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
	friend class FileTransferManager;
	QScopedPointer<FileTransferEnginePrivate> d_ptr;
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
	void error(qutim_sdk_0_3::FileTransferEngine::Error);
};

class LIBQUTIM_EXPORT FileTransferFactory : public QObject
{
	Q_OBJECT
public:
	explicit FileTransferFactory();
	virtual bool check(ChatUnit *unit) = 0;
	virtual FileTransferEngine *create(ChatUnit *unit) = 0;
};

class LIBQUTIM_EXPORT FileTransferManager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(FileTransferManager)
public:
	explicit FileTransferManager();
	virtual ~FileTransferManager();
	bool checkAbility(ChatUnit *unit);
	virtual void sendFile(ChatUnit *unit, const QString &fileName) = 0;
signals:
	void engineCreated(FileTransferEngine *);
protected:
	FileTransferEngine *getEngine(ChatUnit *unit, FileTransferEngine *last = 0);
	QScopedPointer<FileTransferManagerPrivate> d_ptr;
};
}

#endif // FILETRANSFER_H
