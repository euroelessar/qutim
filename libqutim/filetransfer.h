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
class FileTransferManagerPrivate;
class FileTransferJobPrivate;
class FileTransferInfoPrivate;
class FileTransferObserverPrivate;
class FileTransferFactoryPrivate;
class FileTransferManagerPrivate;

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
	
	FileTransferJob(ChatUnit *unit, Direction direction, FileTransferFactory *factory);
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

#endif // FILETRANSFER_H
