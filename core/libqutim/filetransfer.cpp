/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "filetransfer.h"
#include "objectgenerator.h"
#include "servicemanager.h"
#include "chatunit.h"
#include "config.h"
#include "metaobjectbuilder.h"
#include "protocol.h"
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QBitArray>

namespace qutim_sdk_0_3
{

#define REMEMBER_ALL_ABILITIES 1

struct FileTransferScope
{
	struct Observer
	{
		Observer();
		QList<QPointer<FileTransferObserver> > list;
		ChatUnit *unit;
#ifdef REMEMBER_ALL_ABILITIES
		QBitArray abilities;
		int setCount;
#else
		bool ability;
#endif
	};
	FileTransferScope() : manager(0), inited(false) {}
	bool init();
	QList<FileTransferFactory*> factories;
	QMap<ChatUnit*, Observer> observers;
	FileTransferManager *manager;
	bool inited;
};
typedef QMap<ChatUnit*, FileTransferScope::Observer> FileTransferObserverMap;

bool FileTransferScope::init()
{
	if (!inited && ObjectGenerator::isInited()) {
		inited = true;
		manager = ServiceManager::getByName<FileTransferManager*>("FileTransferManager");
	}
	return manager != NULL;
}

static QList<FileTransferFactory*> sortFactories(const QStringList &names,
												 const QList<FileTransferFactory*> &factories)
{
	// Put all factories in a hash with class name as the key.
	QMultiHash<QString, FileTransferFactory*> factoriesHash;
	foreach (FileTransferFactory *factory, factories)
		factoriesHash.insert(factory->metaObject()->className(), factory);

	// Sort factories
	QList<FileTransferFactory*> newFactories;
	foreach (const QString &name, names) {
		FileTransferFactory *factory = factoriesHash.take(name);
		if (factory)
			newFactories.push_back(factory);
	}
	newFactories += factoriesHash.values();
	Q_ASSERT(newFactories.count() == factories.count());
	return newFactories;
}

Q_GLOBAL_STATIC(FileTransferScope, scope)

FileTransferScope::Observer::Observer()
    : abilities(scope()->factories.size()),
#ifdef REMEMBER_ALL_ABILITIES
      setCount(0)
#else
      ability(0)
#endif
{
}

class FileTransferManagerPrivate
{
	Q_DECLARE_PUBLIC(FileTransferManager)
public:
	FileTransferManagerPrivate(FileTransferManager *q) : q_ptr(q) {}
	static FileTransferManagerPrivate *get(FileTransferManager *q) { return q->d_func(); }
	void handleJob(FileTransferJob *job, FileTransferJob *oldJob) { q_func()->handleJob(job, oldJob); }
	FileTransferManager *q_ptr;
};

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
	Q_DECLARE_PUBLIC(FileTransferJob)
public:
	FileTransferJobPrivate(FileTransferJob::Direction d, FileTransferJob *q) :
		accepted(d == FileTransferJob::Outgoing),
		direction(d), error(FileTransferJob::NoError),
		state(FileTransferJob::Initiation), currentIndex(-1),
		progress(0), fileProgress(0), totalSize(0), q_ptr(q),
		skipToNextFactoryAtError(true)
	{}
	void addFile(const QFileInfo &info);
	QIODevice *device(int index);
	ChatUnit *unit;
	QString title;
	bool accepted;
	FileTransferJob::Direction direction;
	FileTransferJob::ErrorType error;
	FileTransferJob::State state;
	LocalizedString errorString;
	LocalizedString stateString;
	QVector<FileTransferInfo> files;
	QVector<QIODevice*> devices;
	int currentIndex;
	qint64 progress;
	qint64 fileProgress;
	qint64 totalSize;
	FileTransferFactory *factory;
	FileTransferJob *q_ptr;
	QDir dir;
	bool skipToNextFactoryAtError;
};

void FileTransferJobPrivate::addFile(const QFileInfo &info)
{
	FileTransferInfo ftInfo;
	ftInfo.setFileName(dir.relativeFilePath(info.absoluteFilePath()));
	ftInfo.setFileSize(info.size());
	files.append(ftInfo);
	totalSize += ftInfo.fileSize();
}

QIODevice *FileTransferJobPrivate::device(int index)
{
	if (!devices[index])
		devices[index] = FileTransferManager::openFile(q_func());
	return devices[index];
}

FileTransferJob::FileTransferJob(ChatUnit *unit, FileTransferJob::Direction direction,
                                 FileTransferFactory *factory) :
    QObject(unit), d_ptr(new FileTransferJobPrivate(direction, this))
{
	Q_D(FileTransferJob);
	d->unit = unit;
	d->factory = factory;
}

FileTransferJob::~FileTransferJob()
{
}

void FileTransferJob::send(const QUrl &url, const QString &title)
{
	Q_D(FileTransferJob);
	QFileInfo info = url.toLocalFile();
	d->title = title.isEmpty() ? info.fileName() : title;
	QStringList files;
	if (info.isDir()) {
		d->dir.setPath(info.absoluteFilePath());
		QDirIterator it(d->dir, QDirIterator::FollowSymlinks | QDirIterator::Subdirectories);
		while (it.hasNext()) {
			it.next();
			info = it.fileInfo();
			if (!info.isFile())
				continue;
			d->addFile(info);
			files << d->files.last().fileName();
		}
	} else {
		d->dir.setPath(info.absolutePath());
		d->addFile(info);
		files << d->files.last().fileName();
	}
	d->devices.resize(files.size());
	doSend();
	//	if (scope()->init())
	//		FileTransferManagerPrivate::get(scope()->manager)->handleJob(this, 0);
}

void FileTransferJob::send(const QDir &baseDir, const QStringList &files, const QString &title)
{
	Q_D(FileTransferJob);
	d->dir = baseDir;
	d->title = title;
	for (int i = 0; i < files.size(); i++) {
		QFileInfo info = d->dir.filePath(files.at(i));
		FileTransferInfo ftInfo;
		ftInfo.setFileName(files.at(i));
		ftInfo.setFileSize(info.size());
		d->files << ftInfo;
	}
	d->devices.resize(files.size());
	doSend();
}

FileTransferJob::Direction FileTransferJob::direction() const
{
	return d_func()->direction;
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

FileTransferInfo FileTransferJob::info(int index) const
{
	return d_func()->files.value(index);
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

LocalizedString FileTransferJob::stateString()
{
	Q_D(FileTransferJob);
	if (!d->stateString.isNull())
		return d->stateString;

	switch (d->state)
	{
	case FileTransferJob::Initiation:
		return tr("Initiation");
	case FileTransferJob::Started:
		return tr("Started");
	case FileTransferJob::Finished:
		return tr("Finished");
	case FileTransferJob::Error:
		return tr("Error");
	default:
		return tr("Unknown");
	}
}

FileTransferJob::ErrorType FileTransferJob::error() const
{
	return d_func()->error;
}

LocalizedString FileTransferJob::errorString()
{
	Q_D(FileTransferJob);
	if (!d->errorString.isNull())
		return d->errorString;

	switch (d->error)
	{
	case FileTransferJob::NetworkError:
		return tr("Network error");
	case FileTransferJob::Canceled:
		return tr("Canceled");
	case FileTransferJob::NotSupported:
		return tr("Not supported");
	case FileTransferJob::IOError:
		return tr("Input/output error");
	default:
		return tr("Unknown");
	}
}

ChatUnit *FileTransferJob::chatUnit() const
{
	return d_func()->unit;
}

bool FileTransferJob::isAccepted()
{
	return d_func()->accepted;
}

void FileTransferJob::stop()
{
	Q_D(FileTransferJob);
	if (d->state != Finished && d->state != Error) {
		doStop();
		setState(Error);
		setError(Canceled);
	}
}

void FileTransferJob::accept()
{
	d_func()->accepted = true;
	doReceive();
	emit accepted();
}

void FileTransferJob::init(int filesCount, qint64 totalSize, const QString &title)
{
	Q_D(FileTransferJob);
	d->files.resize(filesCount);
	d->devices.resize(filesCount);
	d->totalSize = totalSize;
	d->title = title;
	emit titleChanged(title);
	emit totalSizeChanged(totalSize);
	if (scope()->init())
		FileTransferManagerPrivate::get(scope()->manager)->handleJob(this, 0);
}

QDir FileTransferJob::baseDir() const
{
	return d_func()->dir;
}

QIODevice *FileTransferJob::setCurrentIndex(int index)
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
	return d->device(index);
}

void FileTransferJob::setFileProgress(qint64 fileProgress)
{
	Q_D(FileTransferJob);
	qint64 delta = fileProgress - d->fileProgress;
	if (fileProgress > 0) fileProgress = 0;
	Q_ASSERT(delta >= 0);
	d->fileProgress = fileProgress;
	d->progress += delta;
	emit progressChanged(d->progress);
}

void FileTransferJob::setError(FileTransferJob::ErrorType err)
{
	Q_D(FileTransferJob);
	if (d->error != err) {
		d->error = err;
		d->errorString = LocalizedString();
		FileTransferJob *job = 0;
		if (d->skipToNextFactoryAtError && d->direction == Outgoing) {
			d->skipToNextFactoryAtError = false;
			QList<FileTransferFactory*> &list = scope()->factories;
			for (int i = list.indexOf(d->factory) + 1; !job && i < list.size(); i++) {
				FileTransferFactory *factory = list.at(i);
				if (factory->checkAbility(d->unit)) {
					job = factory->create(d->unit);
					FileTransferJobPrivate *p = job->d_func();
					p->files = d->files;
					p->dir = d->dir;
					p->devices.resize(p->files.size());
					FileTransferManagerPrivate::get(scope()->manager)->handleJob(job, this);
					job->doSend();
				}
			}
		}
		emit error(d->error, job);
		emit errorStringChanged(errorString());
	}
}

void FileTransferJob::setErrorString(const LocalizedString &error)
{
	d_func()->errorString = error;
	emit errorStringChanged(errorString());
}

void FileTransferJob::setState(FileTransferJob::State state)
{
	Q_D(FileTransferJob);
	if (d->state != state) {
		d->state = state;
		d->stateString = LocalizedString();
		emit stateChanged(state);
		emit stateStringChanged(stateString());
	}
}

void FileTransferJob::setStateString(const LocalizedString &state)
{
	d_func()->stateString = state;
	emit stateStringChanged(stateString());
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

class FileTransferObserverPrivate
{
	Q_DECLARE_PUBLIC(FileTransferObserver)
public:
	FileTransferObserverPrivate(FileTransferObserver *q) : q_ptr(q), scope(0) {}
	static FileTransferObserverPrivate *get(FileTransferObserver *o) { return o->d_func(); }
	void emitAbilityChanged(bool ability) { emit q_func()->abilityChanged(ability); }
	void _q_clearObserverData(QObject *obj);
	FileTransferObserver *q_ptr;
	FileTransferObserverMap::Iterator scope;
	bool isEmpty;
};

void FileTransferObserverPrivate::_q_clearObserverData(QObject *unit)
{
	Q_Q(FileTransferObserver);
	scope->list.removeOne(q);
	if (scope->list.isEmpty()) {
		if (unit) {
			QList<FileTransferFactory*> &list = qutim_sdk_0_3::scope()->factories;
			for (int i = 0; i < list.size(); i++)
				list.at(i)->stopObserve(reinterpret_cast<ChatUnit*>(unit));
		}
		qutim_sdk_0_3::scope()->observers.erase(scope);
	}
	isEmpty = true;
}

FileTransferObserver::FileTransferObserver(ChatUnit *unit) :
    d_ptr(new FileTransferObserverPrivate(this))
{
	Q_D(FileTransferObserver);
	d->scope = scope()->observers.find(unit);
	if (d->scope == scope()->observers.end())
		d->scope = scope()->observers.insert(unit, FileTransferScope::Observer());
	if (d->scope->list.isEmpty()) {
		d->scope->unit = unit;
		QList<FileTransferFactory*> &list = scope()->factories;
#ifdef REMEMBER_ALL_ABILITIES
		d->scope->setCount = 0;
		d->scope->abilities.resize(list.size());
		for (int i = 0; i < list.size(); i++) {
			list.at(i)->startObserve(unit);
			if (list.at(i)->checkAbility(unit)) {
				++d->scope->setCount;
				d->scope->abilities.setBit(i, true);
			}
		}
#else
		for (int i = 0; !d->scope->ability && i < list.size(); i++) {
			list.at(i)->startObserve(unit);
			d->scope->ability |= list.at(i)->checkAbility(unit);
		}
#endif
	}
	d->scope->list.append(this);
	connect(unit, SIGNAL(destroyed(QObject*)),
			SLOT(_q_clearObserverData(QObject*)));
	d->isEmpty = false;
}

FileTransferObserver::~FileTransferObserver()
{
	Q_D(FileTransferObserver);
	if (!d->isEmpty)
		d->_q_clearObserverData(d->scope->unit);
}

bool FileTransferObserver::checkAbility() const
{
	Q_D(const FileTransferObserver);
	if (d->isEmpty)
		return false;
#ifdef REMEMBER_ALL_ABILITIES
	return d->scope->setCount > 0;
#else
	return d->scope->ability;
#endif
}

ChatUnit *FileTransferObserver::chatUnit() const
{
	Q_D(const FileTransferObserver);
	return d->isEmpty ? 0 : d->scope->unit;
}

class FileTransferFactoryPrivate
{
public:
	LocalizedString name;
	LocalizedString description;
	QIcon icon;
	FileTransferFactory::Capabilities capabilities;
};

FileTransferFactory::FileTransferFactory(const LocalizedString &name,
										 FileTransferFactory::Capabilities capabilities) :
    d_ptr(new FileTransferFactoryPrivate)
{
	Q_D(FileTransferFactory);
	d->capabilities = capabilities;
	d->name = name;
}

FileTransferFactory::~FileTransferFactory()
{
}

FileTransferFactory::Capabilities FileTransferFactory::capabilities() const
{
	return d_func()->capabilities;
}

LocalizedString FileTransferFactory::name() const
{
	return d_func()->name;
}

LocalizedString FileTransferFactory::description() const
{
	return d_func()->description;
}

QIcon FileTransferFactory::icon() const
{
	return d_func()->icon;
}

void FileTransferFactory::changeAvailability(ChatUnit *unit, bool canSend)
{
	FileTransferScope::Observer &scp = scope()->observers[unit];
#ifdef REMEMBER_ALL_ABILITIES
	QList<FileTransferFactory*> &list = scope()->factories;
	int index = list.indexOf(this);
	bool changed = scp.abilities.testBit(index) != canSend;
	if (!changed)
		return;
	scp.setCount -= (scp.abilities.toggleBit(index) << 1) - 1;
	if (scp.setCount == 0 || (scp.setCount == 1 && canSend)) {
		for (int i = 0; i < scp.list.size(); i++) {
			FileTransferObserver *obs = scp.list.at(i).data();
			if (obs)
				FileTransferObserverPrivate::get(obs)->emitAbilityChanged(scp.setCount > 0);
		}
	}
#else
	if (!scp.ability && canSend) {
		scp.ability = true;
	} else if (scp.ability && !canSend) {
		// Need recalc abilities
		bool oldAbility = scp.ability;
		scp.ability = false;
		QList<FileTransferFactory*> &list = scope()->factories;
		for (int i = 0; !scp.ability && i < list.size(); i++)
			scp.ability |= list.at(i)->checkAbility(scp.unit);
		if (scp.ability == oldAbility)
			return;
	} else {
		return;
	}
	for (int i = 0; i < scp.list.size(); i++) {
		FileTransferObserver *obs = scp.list.at(i);
		FileTransferObserverPrivate::get(obs)->emitAbilityChanged(scp.ability);
	}
#endif
}

void FileTransferFactory::setDescription(const LocalizedString &description)
{
	d_func()->description = description;
}

void FileTransferFactory::setIcon(const QIcon &icon)
{
	d_func()->icon = icon;
}

void FileTransferFactory::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}

FileTransferManager::FileTransferManager() : d_ptr(new FileTransferManagerPrivate(this))
{
	scope()->manager = this;
	scope()->inited = true;
	
	// TODO: Think, may be we should move such checks to ModuleManager?
	QList<QLatin1String> protocols;
	foreach (Protocol *protocol, Protocol::all())
		protocols.append(QLatin1String(protocol->metaObject()->className()));
	
	foreach (const ObjectGenerator *gen, ObjectGenerator::module<FileTransferFactory>()) {
		const char *proto = MetaObjectBuilder::info(gen->metaObject(), "DependsOn");
		if (qstrlen(proto) > 0 && !protocols.contains(QLatin1String(proto)))
			continue;
		scope()->factories << gen->generate<FileTransferFactory>();
	}
	QStringList names = Config().value("filetransfer/factories").toStringList();
	if (!names.isEmpty())
		scope()->factories = sortFactories(names, scope()->factories);
}

FileTransferManager::~FileTransferManager()
{
}

bool FileTransferManager::checkAbility(ChatUnit *unit)
{
	QMap<ChatUnit*,FileTransferScope::Observer>::iterator it = scope()->observers.find(unit);
	if (it != scope()->observers.end()) {
#ifdef REMEMBER_ALL_ABILITIES
		return it.value().setCount > 0;
#else
		return it.value().ability;
#endif
	}
	QList<FileTransferFactory*> &list = scope()->factories;
	bool ok = false;
	for (int i = 0; !ok && i < list.size(); i++)
		ok |= list.at(i)->checkAbility(unit);
	return ok;
}

FileTransferJob *FileTransferManager::send(ChatUnit *unit, const QUrl &url, const QString &title, FileTransferFactory *factory)
{
	if (!scope()->init())
		return 0;

	bool specificFactory = factory;
	if (!factory) {
		QList<FileTransferFactory*> &list = scope()->factories;
		for (int i = 0; i < list.size(); i++) {
			FileTransferFactory *current = list.at(i);
			if (current->checkAbility(unit)) {
				factory = current;
				break;
			}
		}
	} else if (!factory->checkAbility(unit)) {
		return 0;
	}

	if (factory) {
		FileTransferJob *job = factory->create(unit);
		scope()->manager->handleJob(job, 0);
		job->d_func()->skipToNextFactoryAtError = !specificFactory;
		job->send(url, title);
		return job;
	}

	return 0;
}

QIODevice *FileTransferManager::openFile(FileTransferJob *job)
{
	if (!scope()->init())
		return 0;
	return scope()->manager->doOpenFile(job);
}

QList<FileTransferFactory*> FileTransferManager::factories()
{
	return scope()->factories;
}

void FileTransferManager::updateFactories(const QStringList &factoryClassNames)
{
	Config().setValue("filetransfer/factories", factoryClassNames);
	if (factoryClassNames.isEmpty())
		return;

	QList<FileTransferFactory*> &oldFactories = scope()->factories;
	QList<FileTransferFactory*> newFactories = sortFactories(factoryClassNames, oldFactories);

#ifdef REMEMBER_ALL_ABILITIES
	int factoriesCount = newFactories.count();

	QVector<int> indexesMap;
	indexesMap.reserve(factoriesCount);
	foreach (FileTransferFactory *factory, oldFactories) {
		int index = newFactories.indexOf(factory);
		Q_ASSERT(index >= 0);
		indexesMap.push_back(index);
	}

	typedef QMap<ChatUnit*, FileTransferScope::Observer>::Iterator Iter;
	Iter itr = scope()->observers.begin();
	Iter end = scope()->observers.end();
	for (; itr != end; ++itr) {
		QBitArray &oldAbilities = itr->abilities;
		QBitArray newAbilities(factoriesCount);
		for (int i = 0; i < factoriesCount; ++i) {
			if (oldAbilities.testBit(i))
				newAbilities.setBit(indexesMap.at(i), true);
		}
		oldAbilities = newAbilities;
	}
#endif

	oldFactories = newFactories;
}

void FileTransferManager::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}

}

#include "moc_filetransfer.cpp"

