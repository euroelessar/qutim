/****************************************************************************
 *  filetransfer.cpp
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

#include "filetransfer.h"
#include "objectgenerator.h"
#include "chatunit.h"
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>

namespace qutim_sdk_0_3
{
class FileTransferInfoPrivate : public QSharedData
{
public:
	FileTransferInfoPrivate() : fileSize(0) {}
	FileTransferInfoPrivate(const FileTransferInfoPrivate &o) :
	    QSharedData(o), fileName(o.fileName), fileSize(o.fileSize) {}
	QString fileName;
	qint64 fileSize;
};

FileTransferInfo::FileTransferInfo() : d_ptr(new FileTransferInfoPrivate)
{
}

FileTransferInfo::FileTransferInfo(const FileTransferInfo &other) : d_ptr(other.d_ptr)
{
}

FileTransferInfo::~FileTransferInfo()
{
}

FileTransferInfo &FileTransferInfo::operator =(const FileTransferInfo &other)
{
	d_ptr = other.d_ptr;
	return *this;
}

QString FileTransferInfo::fileName() const
{
	return d_ptr->fileName;
}

void FileTransferInfo::setFileName(const QString &fileName)
{
	d_ptr->fileName = fileName;
}

qint64 FileTransferInfo::fileSize() const
{
	return d_ptr->fileSize;
}

void FileTransferInfo::setFileSize(qint64 fileSize)
{
	d_ptr->fileSize = fileSize;
}

class FileTransferJobPrivate
{
public:
	FileTransferJobPrivate(FileTransferJob::Direction d) :
	    direction(d), error(FileTransferJob::NoError),
	    state(FileTransferJob::Initiation), currentIndex(-1),
	    progress(0), fileProgress(0), totalSize(0) {}
	void addFile(const QDir &dir, const QFileInfo &info);
	QString title;
	FileTransferJob::Direction direction;
	FileTransferJob::ErrorType error;
	FileTransferJob::State state;
	QVector<FileTransferInfo> files;
	int currentIndex;
	qint64 progress;
	qint64 fileProgress;
	qint64 totalSize;
};

void FileTransferJobPrivate::addFile(const QDir &dir, const QFileInfo &info)
{
	FileTransferInfo ftInfo;
	ftInfo.setFileName(dir.relativeFilePath(info.absoluteFilePath()));
	ftInfo.setFileSize(info.size());
	files.append(ftInfo);
	totalSize += ftInfo.fileSize();
}

FileTransferJob::FileTransferJob(ChatUnit *unit, FileTransferJob::Direction direction) :
    QObject(unit), d_ptr(new FileTransferJobPrivate(direction))
{
}

FileTransferJob::~FileTransferJob()
{
}

void FileTransferJob::send(const QUrl &url, const QString &title)
{
	Q_D(FileTransferJob);
	d->title = title;
	QFileInfo info = url.toLocalFile();
	QDir dir;
	QStringList files;
	if (info.isDir()) {
		dir.setPath(info.absoluteFilePath());
		QDirIterator it(dir, QDirIterator::FollowSymlinks | QDirIterator::Subdirectories);
		while (it.hasNext()) {
			it.next();
			info = it.fileInfo();
			d->addFile(dir, info);
			files << d->files.last().fileName();
		}
	} else {
		dir.setPath(info.absolutePath());
		d->addFile(dir, info);
		files << d->files.last().fileName();
	}
	doSend(dir, files);
}

void FileTransferJob::stop()
{
	doStop();
}

QString FileTransferJob::title() const
{
	return d_func()->title;
}

QString FileTransferJob::fileName() const
{
	Q_D(const FileTransferJob);
	return d->currentIndex == -1 ? QString() : d->files.at(d->currentIndex).fileName();
}

int FileTransferJob::filesCount() const
{
	return d_func()->files.size();
}

int FileTransferJob::currentIndex() const
{
	return d_func()->currentIndex;
}

qint64 FileTransferJob::fileSize() const
{
	Q_D(const FileTransferJob);
	return d->currentIndex == -1 ? 0 : d->files.at(d->currentIndex).fileSize();
}

qint64 FileTransferJob::progress() const
{
	return d_func()->progress;
}

qint64 FileTransferJob::totalSize() const
{
	return d_func()->totalSize;
}

FileTransferJob::State FileTransferJob::state() const
{
	return d_func()->state;
}

FileTransferJob::ErrorType FileTransferJob::error() const
{
	return d_func()->error;
}

void FileTransferJob::init(int filesCount, qint64 totalSize, const QString &title)
{
	Q_D(FileTransferJob);
	d->files.resize(filesCount);
	d->totalSize = totalSize;
	d->title = title;
	emit titleChanged(title);
	emit totalSizeChanged(totalSize);
}

void FileTransferJob::setCurrentIndex(int index)
{
	Q_D(FileTransferJob);
	if (d->currentIndex != index) {
		d->currentIndex = index;
		d->fileProgress = 0;
		const FileTransferInfo &info = d->files.at(index);
		emit currentIndexChanged(index);
		emit fileNameChanged(info.fileName());
		emit fileSizeChanged(info.fileSize());
	}
}

void FileTransferJob::setFileProgress(qint64 fileProgress)
{
	Q_D(FileTransferJob);
	qint64 delta = fileProgress - d->fileProgress;
	Q_ASSERT(delta > 0);
	d->fileProgress = fileProgress;
	d->progress += delta;
	emit progressChanged(d->progress);
}

void FileTransferJob::setError(FileTransferJob::ErrorType err)
{
	Q_D(FileTransferJob);
	if (d->error != err) {
		d->error = err;
		emit error(d->error);
	}
}

void FileTransferJob::setState(FileTransferJob::State state)
{
	Q_D(FileTransferJob);
	if (d->state != state) {
		d->state = state;
		emit stateChanged(state);
	}
}

void FileTransferJob::setFileInfo(int index, const FileTransferInfo &info)
{
	Q_D(FileTransferJob);
	FileTransferInfo old = info;
	qSwap(d->files[index], old);
	if (index == d->currentIndex) {
		if (old.fileName() != info.fileName())
			emit fileNameChanged(info.fileName());
		if (old.fileSize() != info.fileSize())
			emit fileSizeChanged(info.fileSize());
	}
}

void FileTransferJob::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}


	class FileTransferEnginePrivate
	{
	public:
		ChatUnit *chatUnit;
		FileTransferFactory *factory;
		FileTransferEngine::Direction direction;
	};

	class FileTransferManagerPrivate
	{
	};

	struct FileTransferData
	{
		QList<FileTransferFactory *> factories;
		QPointer<FileTransferManager> manager;
	};

	void init(FileTransferData *data)
	{
		GeneratorList gens = ObjectGenerator::module<FileTransferFactory>();
		foreach(const ObjectGenerator *gen, gens)
			data->factories << gen->generate<FileTransferFactory>();
		gens = ObjectGenerator::module<FileTransferManager>();
		if (!gens.isEmpty())
			data->manager = gens.first()->generate<FileTransferManager>();
	}

	Q_GLOBAL_STATIC_WITH_INITIALIZER(FileTransferData, data, init(x.data()));

	FileTransferEngine::FileTransferEngine(ChatUnit *chatUnit, Direction direction, FileTransferFactory *factory) :
			QObject(factory), d_ptr(new FileTransferEnginePrivate)
	{
		Q_D(FileTransferEngine);
		d->chatUnit = chatUnit;
		d->factory = factory;
		d->direction = direction;
	}

	FileTransferEngine::~FileTransferEngine()
	{
	}

	ChatUnit *FileTransferEngine::chatUnit() const
	{
		return d_func()->chatUnit;
	}

	int FileTransferEngine::localPort() const
	{
		return -1;
	}

	int FileTransferEngine::remotePort() const
	{
		return -1;
	}

	QHostAddress FileTransferEngine::remoteAddress() const
	{
		return QHostAddress();
	}

	FileTransferEngine::Direction FileTransferEngine::direction() const
	{
		return d_func()->direction;
	}

	void FileTransferEngine::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}

	FileTransferFactory::FileTransferFactory()
	{
	}

	FileTransferFactory::~FileTransferFactory()
	{
	}

	FileTransferManager::FileTransferManager() : d_ptr(new FileTransferManagerPrivate)
	{
	}

	FileTransferManager::~FileTransferManager()
	{
	}

	FileTransferManager *FileTransferManager::instance()
	{
		return data()->manager;
	}

	bool FileTransferManager::checkAbility(ChatUnit *unit)
	{
		foreach (FileTransferFactory *factory, data()->factories) {
			if (factory->check(unit))
				return true;
		}
		return false;
	}

	FileTransferEngine *FileTransferManager::getEngine(ChatUnit *unit, FileTransferEngine *last)
	{
		if (last && last->direction() == FileTransferEngine::Receive)
			return 0;

		FileTransferFactory *lastFactory = last ? last->d_func()->factory : 0;
		FileTransferData *d = data();
		int index = lastFactory ? d->factories.indexOf(lastFactory) : -1;
		index++;
		for (; index < d->factories.size(); index++) {
			if (d->factories.at(index)->check(unit))
				return d->factories.at(index)->create(unit);
		}
		return 0;
	}
}
