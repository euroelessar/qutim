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
#include <QSharedData>

class QDir;
class QUrl;

namespace qutim_sdk_0_3
{
class ChatUnit;
class LocalizedString;
class FileTransferFactory;
class FileTransferManager;
class FileTransferEnginePrivate;
class FileTransferManagerPrivate;
class FileTransferJobPrivate;
class FileTransferInfoPrivate;
class FileTransferObserverPrivate;
namespace Games
{
class FileTransferFactory;
}
class LIBQUTIM_EXPORT FileTransferInfo
{
public:
	FileTransferInfo();
	FileTransferInfo(const FileTransferInfo &other);
	~FileTransferInfo();
	FileTransferInfo &operator =(const FileTransferInfo &other);
	
	// Relative file name
	QString fileName() const;
	void setFileName(const QString &fileName);
	qint64 fileSize() const;
	void setFileSize(qint64 fileSize);
private:
	QSharedDataPointer<FileTransferInfoPrivate> d_ptr;
};

class LIBQUTIM_EXPORT FileTransferJob : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(FileTransferJob)
	Q_ENUMS(Direction ErrorType State)
	Q_PROPERTY(QString title READ title NOTIFY titleChanged)
	Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
	Q_PROPERTY(int filesCount READ filesCount)
	Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)
	Q_PROPERTY(qint64 totalSize READ totalSize NOTIFY totalSizeChanged)
	Q_PROPERTY(qint64 fileSize READ fileSize NOTIFY fileSizeChanged)
	Q_PROPERTY(qint64 progress READ progress NOTIFY progressChanged)
	Q_PROPERTY(qutim_sdk_0_3::FileTransferJob::State state READ state NOTIFY stateChanged)
public:
	enum Direction { Outgoing, Incoming };
	enum ErrorType { NetworkError, Canceled, NotSupported, NoError };
	enum State { Initiation, Started, Finished, Error };
	
	FileTransferJob(ChatUnit *unit, Direction direction, Games::FileTransferFactory *factory);
	virtual ~FileTransferJob();
	
	// Send some file or dir
	void send(const QUrl &url, const QString &title = QString());
	void send(const QDir &baseDir, const QStringList &files, const QString &title);
	void stop();
	Direction direction() const;
	QString title() const;
	QString fileName() const;
	// For outgoing only
	QDir baseDir() const;
	FileTransferInfo info(int index) const;
	int filesCount() const;
	int currentIndex() const;
	qint64 fileSize() const;
	qint64 progress() const;
	qint64 totalSize() const;
	State state() const;
	ErrorType error() const;
protected:
	virtual void doSend() = 0;
	virtual void doStop() = 0;
	// For incoming only
	void init(int filesCount, qint64 totalSize, const QString &title);
	// Device for local files to read/write
	QIODevice *setCurrentIndex(int index);
	void setFileProgress(qint64 fileProgress);
	void setError(ErrorType error);
	void setState(State state);
	void setFileInfo(int index, const FileTransferInfo &info);
	virtual void virtual_hook(int id, void *data);
signals:
#if !defined(Q_MOC_RUN) && !defined(DOXYGEN_SHOULD_SKIP_THIS) && !defined(IN_IDE_PARSER)
private: // don't tell moc, doxygen or kdevelop, but those signals are in fact private
#endif
	void titleChanged(const QString &);
	void fileNameChanged(const QString &);
	void fileSizeChanged(qint64);
	void progressChanged(qint64);
	void totalSizeChanged(qint64);
	void currentIndexChanged(int);
	void error(qutim_sdk_0_3::FileTransferJob::ErrorType, qutim_sdk_0_3::FileTransferJob *newJob);
	void stateChanged(qutim_sdk_0_3::FileTransferJob::State);
	void finished();
private:
	QScopedPointer<FileTransferJobPrivate> d_ptr;
};

class LIBQUTIM_EXPORT FileTransferObserver : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(FileTransferObserver)
	Q_PROPERTY(bool ability READ checkAbility NOTIFY abilityChanged)
public:
	FileTransferObserver(ChatUnit *unit);
	~FileTransferObserver();
	bool checkAbility() const;
signals:
	void abilityChanged(bool);
protected:
	QScopedPointer<FileTransferObserverPrivate> d_ptr;
};

namespace Games
{
class FileTransferFactoryPrivate;
class FileTransferManagerPrivate;

class LIBQUTIM_EXPORT FileTransferFactory : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(FileTransferFactory)
	Q_ENUMS(Capability)
public:
	enum Capability {
		CanSendMultiple = 0x01
	};
	Q_DECLARE_FLAGS(Capabilities, Capability)
	FileTransferFactory(const LocalizedString &name, Capabilities capabilities);
	~FileTransferFactory();
	Capabilities capabilities() const;
	virtual bool checkAbility(ChatUnit *unit) = 0;
	virtual bool startObserve(ChatUnit *unit) = 0;
	virtual bool stopObserve(ChatUnit *unit) = 0;
	virtual FileTransferJob *create(ChatUnit *unit) = 0;
protected:
	void changeAvailability(ChatUnit *unit, bool canSend);
	virtual void virtual_hook(int id, void *data);
	QScopedPointer<FileTransferFactoryPrivate> d_ptr;
};

class LIBQUTIM_EXPORT FileTransferManager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(FileTransferManager)
	Q_CLASSINFO("Service", "FileTransferManager")
public:
	FileTransferManager();
	~FileTransferManager();
	
	static bool checkAbility(ChatUnit *unit);
	static FileTransferJob *send(ChatUnit *unit, const QUrl &url, const QString &title = QString());
	static QIODevice *openFile(FileTransferJob *job);
protected:
	virtual QIODevice *doOpenFile(FileTransferJob *job) = 0;
	virtual void handleJob(FileTransferJob *job, FileTransferJob *oldJob) = 0;
	virtual void virtual_hook(int id, void *data);
	QScopedPointer<FileTransferManagerPrivate> d_ptr;
};
}

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
