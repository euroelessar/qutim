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
#include <QStringList>

class QUrl;

namespace qutim_sdk_0_3
{
class ChatUnit;
class FileTransferFactory;
class FileTransferManager;
class FileTransferEnginePrivate;
class FileTransferManagerPrivate;
class FileTransferJobPrivate;

//class FileTransferJob : public QObject
//{
//	Q_OBJECT
//	Q_DECLARE_PRIVATE(FileTransferJob)
//	Q_ENUMS(Direction ErrorType State)
//	Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
//	Q_PROPERTY(int filesCount READ filesCount)
//	Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)
//	Q_PROPERTY(int fileSize READ fileSize NOTIFY fileSizeChanged)
//	Q_PROPERTY(int filePosition READ filePosition NOTIFY filePositionChanged)
//	Q_PROPERTY(qutim_sdk_0_3::FileTransferJob::State state READ state NOTIFY stateChanged)
//public:
//	enum Direction { Send, Receive };
//	enum ErrorType { NetworkError, Canceled, NotSupported };
//	enum State { Initiation, Started, Finished, Error };
	
//	FileTransferJob(ChatUnit *unit, Direction direction);
//	virtual ~FileTransferJob();
	
//	// Some file/dir
//	virtual void send(const QUrl &url) = 0;
//	QString fileName() const;
//	int filesCount() const;
//	int currentIndex() const;
//	virtual int fileSize() = 0;
//	virtual int filePosition() = 0;
//	State state() const;
//protected:
//	void setState(State state);
//	void setFileName(int index, const QString &name);
//signals:
//	void fileNameChanged(const QString &);
//	void fileNameChanged(int);
//	void error(qutim_sdk_0_3::FileTransferJob::ErrorType);
//	void stateChanged(qutim_sdk_0_3::FileTransferJob::State);
//	void finished();
//private:
//	QScopedPointer<FileTransferJobPrivate> d_ptr;
//};


class LIBQUTIM_EXPORT FileTransferEngine : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(FileTransferEngine)
	// Total progress
	Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
	Q_PROPERTY(int currentFile READ currentFile NOTIFY currentFileChanged)
	// Local and remote filenames should be constant
	Q_PROPERTY(QStringList files READ files WRITE setFiles NOTIFY filesChanged)
	Q_PROPERTY(QStringList remoteFiles READ remoteFiles NOTIFY remoteFilesChanged)
	// Size of current file
	Q_PROPERTY(qint64 fileSize READ fileSize NOTIFY fileSizeChanged)
	// Size of all files
	Q_PROPERTY(qint64 totalSize READ totalSize NOTIFY totalSizeChanged)
	Q_PROPERTY(int localPort READ localPort NOTIFY localPortChanged)
	Q_PROPERTY(int remotePort READ remotePort NOTIFY remotePortChanged)
	Q_PROPERTY(QHostAddress remoteAddress READ remoteAddress NOTIFY remoteAddressChanged)
	Q_PROPERTY(Direction direction READ direction CONSTANT)
	Q_PROPERTY(State state READ state NOTIFY stateChanged)
	Q_PROPERTY(ChatUnit* chatUnit READ chatUnit CONSTANT)
public:
	// Default should be Send, Receive must be setted only by impl itself.
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
	explicit FileTransferEngine(ChatUnit *, Direction, FileTransferFactory *);
	virtual ~FileTransferEngine();
	ChatUnit *chatUnit() const;
	virtual int progress() const = 0;
	virtual int currentFile() const = 0;
	virtual QStringList files() const = 0;
	virtual QStringList remoteFiles() const = 0;
	virtual void setFiles(const QStringList &) = 0;
	virtual qint64 totalSize() const = 0;
	virtual qint64 fileSize() const = 0;
	virtual int localPort() const;
	virtual int remotePort() const;
	virtual QHostAddress remoteAddress() const;
	Direction direction() const;
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
	void currentFileChanged(int);
	// This means that user has selected places for files to hold,
	// engine should start transfer
	void filesChanged(const QStringList &);
	// If direction is Sending UI should ask user for files
	void remoteFilesChanged(const QStringList &);
	void fileSizeChanged(qint64);
	void totalSizeChanged(qint64);
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
	FileTransferFactory();
	~FileTransferFactory();
	// If it is possible to send file to this unit
	virtual bool check(ChatUnit *unit) = 0;
	// Create Engine for file sending, not receiving
	virtual FileTransferEngine *create(ChatUnit *unit) = 0;
};

class LIBQUTIM_EXPORT FileTransferManager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(FileTransferManager)
public:
	explicit FileTransferManager();
	virtual ~FileTransferManager();
	static FileTransferManager *instance();
	bool checkAbility(ChatUnit *unit);
	virtual void send(ChatUnit *unit, const QStringList &files) = 0;
	inline void send(ChatUnit *unit, const QString &file) { send(unit, QStringList(file)); }
	// For incoming file transfer
	virtual void receive(FileTransferEngine *engine) = 0;
signals:
	void engineCreated(FileTransferEngine *);
protected:
	FileTransferEngine *getEngine(ChatUnit *unit, FileTransferEngine *last = 0);
	QScopedPointer<FileTransferManagerPrivate> d_ptr;
};
}

#endif // FILETRANSFER_H
