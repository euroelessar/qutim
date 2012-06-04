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

#include "config.h"
#include "cryptoservice.h"
#include "systeminfo.h"
#include "metaobjectbuilder.h"
#include "debug.h"
#include <QSet>
#include <QStringList>
#include <QFileInfo>
#include <QDateTime>
#include <QEvent>
#include <QCoreApplication>
#include <QBasicTimer>

#define CONFIG_MAKE_DIRTY_ONLY_AT_SET_VALUE 1

namespace qutim_sdk_0_3
{
Q_GLOBAL_STATIC(QList<ConfigBackend*>, all_config_backends)
LIBQUTIM_EXPORT QList<ConfigBackend*> &get_config_backends()
{ return *all_config_backends(); }

struct ConfigAtom : public QSharedData
{
	typedef QExplicitlySharedDataPointer<ConfigAtom> Ptr;
	inline ConfigAtom() : deleteOnDestroy(true), typeMap(true), readOnly(false), list(0)
	{
	}
	ConfigAtom(QVariant &var, bool isMap);
	ConfigAtom(const ConfigAtom &o)
		: QSharedData(o), deleteOnDestroy(o.deleteOnDestroy), typeMap(o.typeMap),
		  readOnly(o.readOnly), map(o.map)
	{
	}
	inline ~ConfigAtom();
	bool deleteOnDestroy;
	bool typeMap;
	bool readOnly;
	union {
		QVariantList *list;
		QVariantMap *map;
	};
};

ConfigAtom::ConfigAtom(QVariant &var, bool isMap) : deleteOnDestroy(false), typeMap(isMap), readOnly(false)
{
	if (isMap && var.type() != QVariant::Map)
		var = QVariantMap();
	else if (!isMap && var.type() != QVariant::List)
		var = QVariantList();

	if (isMap)
		map = reinterpret_cast<QVariantMap*>(var.data());
	else
		list = reinterpret_cast<QVariantList*>(var.data());
}

ConfigAtom::~ConfigAtom()
{
	if (deleteOnDestroy && typeMap)
		delete map;
	else if (deleteOnDestroy && !typeMap)
		delete list;
}

struct ConfigLevel : public QSharedData
{
	typedef QExplicitlySharedDataPointer<ConfigLevel> Ptr;
	inline ConfigLevel() : arrayElement(false)
	{
	}
	inline ~ConfigLevel()
	{
	}

	QList<ConfigAtom::Ptr> atoms;
	bool arrayElement;
};

class ConfigSource : public QSharedData
{
	Q_DISABLE_COPY(ConfigSource)
public:
	typedef QExplicitlySharedDataPointer<ConfigSource> Ptr;
	inline ConfigSource() : backend(0), dirty(false), isAtLoop(false), data(new ConfigAtom)
	{
	}
	inline ~ConfigSource()
	{
		if (dirty) sync();
	}
	static ConfigSource::Ptr open(const QString &path, bool systemDir, bool create, ConfigBackend *bcknd = 0);
	inline void update() { lastModified = QFileInfo(fileName).lastModified(); }
	bool isValid() {
		//FIXME Elessar it's doesn't work!
		//QFileInfo info(fileName);
		//if (!info.exists())
		//	return false;
		//else
			return QFileInfo(fileName).lastModified() == lastModified;
	}
	void sync();
	void makeDirty() { dirty = true; /*Q_ASSERT(!"Haha! I've caught you!");*/ }
	QString fileName;
	ConfigBackend *backend;
	bool dirty;
	bool isAtLoop;
	ConfigAtom::Ptr data;
	QDateTime lastModified;
};

class ConfigSourceHash : public QObject
{
public:
	ConfigSource::Ptr value(const QString &key)
	{
		Hash::Iterator it = m_hash.find(key);
		if (it == m_hash.end())
			return ConfigSource::Ptr();
		m_timers.remove(it->timer.timerId());
		it->timer.start(5 * 60, this);
		m_timers.insert(it->timer.timerId(), key);
		return it->config;
	}

	void insert(const QString &key, const ConfigSource::Ptr &value)
	{
		Hash::Iterator it = m_hash.find(key);
		if (it == m_hash.end())
			it = m_hash.insert(key, Info());
		m_timers.remove(it->timer.timerId());
		it->timer.start(5 * 60, this);
		it->config = value;
		m_timers.insert(it->timer.timerId(), key);
	}

	void timerEvent(QTimerEvent *event)
	{
		QString key = m_timers.take(event->timerId());
		m_hash.remove(key);
	}

private:
	struct Info
	{
		mutable QBasicTimer timer;
		ConfigSource::Ptr config;
	};

	typedef QHash<QString, Info> Hash;
	Hash m_hash;
	mutable QHash<int, QString> m_timers;
};

Q_GLOBAL_STATIC(ConfigSourceHash, sourceHash)

ConfigSource::Ptr ConfigSource::open(const QString &path, bool systemDir, bool create, ConfigBackend *backend)
{
	QString fileName = path;
	if (fileName.isEmpty())
		fileName = QLatin1String("profile");
	QFileInfo info(fileName);
	if (!info.isAbsolute()) {
		SystemInfo::DirType type = systemDir
				? SystemInfo::SystemConfigDir
				: SystemInfo::ConfigDir;
		fileName = SystemInfo::getDir(type).filePath(fileName);
	} else if (systemDir) {
		// We need to open absolute dir only once
		return ConfigSource::Ptr();
	}
	fileName = QDir::cleanPath(fileName);

	ConfigSource::Ptr result = sourceHash()->value(fileName);
	if (result && result->isValid())
		return result;
	
	const QList<ConfigBackend*> &backends = *all_config_backends();
	if (!backend) {
		QByteArray suffix = info.suffix().toLatin1().toLower();
	
		if (backends.isEmpty())
			return ConfigSource::Ptr();
	
		if (!suffix.isEmpty()) {
			for (int i = 0; i < backends.size(); i++) {
				if (backends.at(i)->name() == suffix) {
					backend = backends.at(i);
					break;
				}
			}
		}
		if (!backend) {
			backend = backends.at(0);
			fileName += QLatin1Char('.');
			fileName += QLatin1String(backend->name());
	
			result = sourceHash()->value(fileName);
			if (result && result->isValid())
				return result;
			info.setFile(fileName);
		}
	}

	if (!info.exists() && !create)
		return result;

	QDir dir = info.absoluteDir();
	if (!dir.exists()) {
		if (!create)
			return result;
		dir.mkpath(info.absolutePath());
	}

	result = new ConfigSource;
	ConfigSource *d = result.data();
	d->backend = backend;
	d->fileName = fileName;
	// QFileInfo says that we can't write to non-exist files but we can
	d->data->readOnly = !info.isWritable() && (systemDir || info.exists());

	d->update();
	QVariant var = d->backend->load(d->fileName);
	if (var.type() == QVariant::Map) {
		d->data->map = new QVariantMap(var.toMap());
	} else if (var.type() == QVariant::List) {
		d->data->typeMap = false;
		d->data->list = new QVariantList(var.toList());
	} else {
		if (!create) {
			d->data->map = 0;
			// result will be cleared automatically
			return ConfigSource::Ptr();
		}
		d->data->map = new QVariantMap();
	}
	sourceHash()->insert(fileName, result);
	return result;
}

class PostConfigSaveEvent : public QEvent
{
public:
	PostConfigSaveEvent(const ConfigSource::Ptr &s) : QEvent(eventType()), source(s) {}
	static Type eventType()
	{
		static Type type = static_cast<Type>(registerEventType());
		return type;
	}
	ConfigSource::Ptr source;
};

class PostConfigSaver : public QObject
{
public:
	PostConfigSaver()
	{
		qAddPostRoutine(cleanup);
	}

	virtual bool event(QEvent *ev)
	{
		if (ev->type() == PostConfigSaveEvent::eventType()) {
			PostConfigSaveEvent *saveEvent = static_cast<PostConfigSaveEvent*>(ev);
			saveEvent->source->sync();
			saveEvent->source->isAtLoop = false;
			return true;
		}
		return QObject::event(ev);
	}
private:
	static void cleanup();
};

Q_GLOBAL_STATIC(PostConfigSaver, postConfigSaver)

void PostConfigSaver::cleanup()
{
	QCoreApplication::sendPostedEvents(postConfigSaver(), PostConfigSaveEvent::eventType());
}

class ConfigPrivate : public QSharedData
{
	Q_DISABLE_COPY(ConfigPrivate)
public:
	inline ConfigPrivate()  { levels << ConfigLevel::Ptr(new ConfigLevel()); }
	inline ~ConfigPrivate() { if (!memoryGuard) sync(); /*qDeleteAll(levels);*/ }
	inline const ConfigLevel::Ptr &current() const { return levels.at(0); }
	void sync();
	void init(const QStringList &paths, const QVariantList &fallbacks, ConfigBackend *backend = 0);
	void init(const QStringList &paths, ConfigBackend *backend = 0);
	QList<ConfigLevel::Ptr> levels;
	QList<ConfigSource::Ptr> sources;
	QExplicitlySharedDataPointer<ConfigPrivate> memoryGuard;
};

void ConfigSource::sync()
{
	const ConfigAtom::Ptr &level = data;
	if (level->typeMap)
		backend->save(fileName, *(level->map));
	else
		backend->save(fileName, *(level->list));
	dirty = false;
	update();
}

void ConfigPrivate::sync()
{
	if (sources.isEmpty())
		return;
	ConfigSource::Ptr source = sources.value(0);
	//		if (source && source->dirty) {
	//			static int evilCounter = 0;
	//			static int goodCounter = 0;
	//			static QMap<QString,int> evilNamedCounter;
	//			static QMap<QString,int> goodNamedCounter;
	//			int evilResult = (++evilNamedCounter[source->fileName]);
	//			int goodResult = (goodNamedCounter[source->fileName] += !source->isAtLoop);
	//			evilCounter++;
	//			goodCounter += !source->isAtLoop;
	//			qDebug("%s %d %d", Q_FUNC_INFO, evilCounter, goodCounter);
	//			qDebug("%s %s %d %d", Q_FUNC_INFO, qPrintable(source->fileName), evilResult, goodResult);
	//		}
	if (source && source->dirty && !source->isAtLoop) {
		source->isAtLoop = true;
		source->dirty = false;
		QCoreApplication::postEvent(postConfigSaver(), new PostConfigSaveEvent(source), -2);
	}
}

void ConfigPrivate::init(const QStringList &paths, ConfigBackend *backend)
{
	init(paths, QVariantList(), backend);
}

void ConfigPrivate::init(const QStringList &paths, const QVariantList &fallbacks, ConfigBackend *backend)
{
	QSet<QString> opened;
	ConfigSource::Ptr source;
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < paths.size(); i++) {
			// Firstly we should open user-specific configs
			source = ConfigSource::open(paths.at(i), j == 1, sources.isEmpty(), backend);
			if (source && !opened.contains(source->fileName)) {
				opened.insert(source->fileName);
				sources << source;
			}
		}
	}
	for (int i = 0; i < sources.size(); i++) {
		source = sources.at(i);
		ConfigAtom::Ptr atom = source->data;
		atom.detach();
		atom->deleteOnDestroy = false;
		atom->readOnly = atom->readOnly || i > 0;
		current()->atoms << atom;
	}
	for (int i = 0; i < fallbacks.size(); ++i) {
		ConfigAtom::Ptr atom(new ConfigAtom());
		QVariant var = fallbacks.at(i);
		if (var.type() == QVariant::Map) {
			atom->map = new QVariantMap(var.toMap());
		} else if (var.type() == QVariant::List) {
			atom->typeMap = false;
			atom->list = new QVariantList(var.toList());
		} else {
			continue;
		}
		atom->deleteOnDestroy = true;
		atom->readOnly = true;
		current()->atoms << atom;
	}
}

Config::Config(const QVariantList &list) : d_ptr(new ConfigPrivate)
{
	Q_D(Config);
	ConfigAtom::Ptr atom(new ConfigAtom());
	atom->typeMap = false;
	atom->deleteOnDestroy = true;
	atom->readOnly = true;
	atom->list = new QVariantList(list);
	d->current()->atoms << atom;
}

Config::Config(QVariantList *list) : d_ptr(new ConfigPrivate)
{
	Q_D(Config);
	ConfigAtom::Ptr atom(new ConfigAtom());
	atom->typeMap = false;
	atom->deleteOnDestroy = false;
	atom->list = list;
	d->current()->atoms << atom;
}

Config::Config(const QVariantMap &map) : d_ptr(new ConfigPrivate)
{
	Q_D(Config);
	ConfigAtom::Ptr atom(new ConfigAtom());
	atom->readOnly = true;
	atom->map = new QVariantMap(map);
	d->current()->atoms << atom;
}

Config::Config(QVariantMap *map) : d_ptr(new ConfigPrivate)
{
	Q_D(Config);
	ConfigAtom::Ptr atom(new ConfigAtom());
	atom->deleteOnDestroy = false;
	atom->map = map;
	d->current()->atoms << atom;
}

Config::Config(const QString &path) : d_ptr(new ConfigPrivate)
{
	Q_D(Config);
	d->init(QStringList(path));
}

Config::Config(const QString &path, ConfigBackend *backend) : d_ptr(new ConfigPrivate)
{
	Q_D(Config);
	d->init(QStringList(path), backend);
}

Config::Config(const QStringList &paths) : d_ptr(new ConfigPrivate)
{
	Q_D(Config);
	d->init(paths);
}

Config::Config(const QString &path, const QVariantList &fallbacks) : d_ptr(new ConfigPrivate)
{
	Q_D(Config);
	d->init(QStringList(path), fallbacks);
}

Config::Config(const QString &path, const QVariant &fallback) : d_ptr(new ConfigPrivate)
{
	Q_D(Config);
	d->init(QStringList(path), QVariantList() << fallback);
}

Config::Config(const Config &other) : d_ptr(other.d_ptr)
{
}

Config &Config::operator =(const Config &other)
{
	d_ptr = other.d_ptr;
	return *this;
}

Config::~Config()
{
}

Config Config::group(const QString &fullName)
{
	Q_D(Config);
	Q_ASSERT(!fullName.isEmpty());
	Config cfg(reinterpret_cast<QVariantMap*>(0));
	ConfigPrivate *p = cfg.d_func();
	p->memoryGuard = d_ptr;
	p->sources = d->sources;
	//		qDeleteAll(p->current()->atoms);
	p->current()->atoms = d->current()->atoms;
	cfg.beginGroup(fullName);
	/*delete*/ p->levels.takeLast();
	return cfg;
}

QStringList Config::childGroups() const
{
	Q_D(const Config);
	QStringList groups;
	QList<ConfigAtom::Ptr> &atoms = d->current()->atoms;
	for (int i = 0; i < atoms.size(); i++) {
		ConfigAtom::Ptr atom = atoms.at(i);
		if (!atom->typeMap)
			continue;
		QVariantMap::iterator it = atom->map->begin();
		for (; it != atom->map->end(); it++) {
			if (it.value().type() == QVariant::Map && !groups.contains(it.key()))
				groups << it.key();
		}
	}
	return groups;
}

QStringList Config::childKeys() const
{
	Q_D(const Config);
	QStringList groups;
	QList<ConfigAtom::Ptr> &atoms = d->current()->atoms;
	for (int i = 0; i < atoms.size(); i++) {
		ConfigAtom::Ptr atom = atoms.at(i);
		if (!atom->typeMap)
			continue;
		QVariantMap::iterator it = atom->map->begin();
		for (; it != atom->map->end(); it++) {
			if (it.value().type() != QVariant::Map && !groups.contains(it.key()))
				groups << it.key();
		}
	}
	return groups;
}

bool Config::hasChildGroup(const QString &name) const
{
	Q_D(const Config);
	QList<ConfigAtom::Ptr> &atoms = d->current()->atoms;
	for (int i = 0; i < atoms.size(); i++) {
		ConfigAtom::Ptr atom = atoms.at(i);
		if (!atom->typeMap)
			continue;
		QVariantMap::iterator it = atom->map->find(name);
		if (it != atom->map->end() && it.value().type() == QVariant::Map)
			return true;
	}
	return false;
}

bool Config::hasChildKey(const QString &name) const
{
	Q_D(const Config);
	QList<ConfigAtom::Ptr> &atoms = d->current()->atoms;
	for (int i = 0; i < atoms.size(); i++) {
		ConfigAtom::Ptr atom = atoms.at(i);
		if (!atom->typeMap)
			continue;
		QVariantMap::iterator it = atom->map->find(name);
		if (it != atom->map->end() && it.value().type() != QVariant::Map)
			return true;
	}
	return false;
}

QStringList parseNames(const QString &fullName)
{
	QStringList names;
	int first = 0;
	do {
		int last = fullName.indexOf('/', first);
		QString name = fullName.mid(first, last != -1 ? last - first : last);
		first = last + 1;
		if (!name.isEmpty())
			names << name;
	} while(first != 0);
	return names;
}

void Config::beginGroup(const QString &fullName)
{
	Q_D(Config);
	Q_ASSERT(!fullName.isEmpty());
	const ConfigLevel::Ptr l(new ConfigLevel());
	QList<ConfigAtom::Ptr> &atoms = d->current()->atoms;
	QStringList names = parseNames(fullName);
	for (int i = 0; i < atoms.size(); i++) {
		ConfigAtom::Ptr current = atoms.at(i);
		Q_ASSERT(current->typeMap);
		if (!current->typeMap)
			continue;
		ConfigAtom::Ptr atom(new ConfigAtom());
		atom->deleteOnDestroy = false;
		atom->readOnly = current->readOnly /*|| i > 0*/;
		atom->map = current->map;
		for (int j = 0; j < names.size(); j++) {
			QVariant &var = (*(atom->map))[names.at(j)];
			if (var.type() != QVariant::Map) {
				if (atom->readOnly) {
					//						/*delete*/ atom;
					atom = 0;
					break;
				} else {
#ifndef CONFIG_MAKE_DIRTY_ONLY_AT_SET_VALUE
					if (!d->sources.isEmpty())
						d->sources.at(0)->makeDirty();
#endif
					var = QVariantMap();
				}
			}
			atom->map = reinterpret_cast<QVariantMap*>(var.data());
		}
		if (atom)
			l->atoms << atom;
	}
	d->levels.prepend(l);
}

void Config::endGroup()
{
	Q_D(Config);
	Q_ASSERT(d->levels.size() > 1);
#ifdef DEBUG		
	const ConfigLevel::Ptr &level = d->levels.at(0);
	const ConfigAtom::Ptr &atom = level->atoms.value(0);
	Q_ASSERT(!atom || atom->typeMap);
#endif
	/*delete*/ d->levels.takeFirst();
}

void Config::remove(const QString &name)
{
	Q_D(Config);
	const ConfigLevel::Ptr &level = d->levels.at(0);
	const ConfigAtom::Ptr atom = level->atoms.value(0);
	Q_ASSERT(!atom || atom->typeMap);
	if (atom && !atom->readOnly) {
		if (atom->map->remove(name) != 0)
			d->sources.at(0)->makeDirty();
	}
}

Config Config::arrayElement(int index)
{
	Q_D(Config);
	Config cfg(reinterpret_cast<QVariantList*>(0));
	ConfigPrivate *p = cfg.d_func();
	p->memoryGuard = d_ptr;
	p->sources = d->sources;
	//		qDeleteAll(p->current()->atoms);
	p->current()->atoms = d->current()->atoms;
	cfg.setArrayIndex(index);
	/*delete */p->levels.takeLast();
	return cfg;
}

int Config::beginArray(const QString &name)
{
	Q_D(Config);
	const ConfigLevel::Ptr l(new ConfigLevel());
	QList<ConfigAtom::Ptr> &atoms = d->current()->atoms;
	int size = 0;
	for (int i = 0; i < atoms.size(); i++) {
		ConfigAtom::Ptr current = atoms.at(i);
		if (current->readOnly) {
			if (current->typeMap) {
				QVariant &var = (*(current->map))[name];
				if (var.type() == QVariant::List) {
					l->atoms << ConfigAtom::Ptr(new ConfigAtom(var, false));
					if (!size)
						size = var.toList().size();
					break;
				}
			}
		} else if (current->typeMap) {
			QVariant &var = (*(current->map))[name];
#ifndef CONFIG_MAKE_DIRTY_ONLY_AT_SET_VALUE
			if (var.type() != QVariant::List && !d->sources.isEmpty())
				d->sources.at(0)->makeDirty(); //Euroelessar, please check this string
#endif
			l->atoms << ConfigAtom::Ptr(new ConfigAtom(var, false));
			if (!size)
				size = var.toList().size();
		}
	}
	d->levels.prepend(l);
	return size;
}

void Config::endArray()
{
	Q_D(Config);
	Q_ASSERT(d->levels.size() > 1);
	const ConfigLevel::Ptr &level = d->levels.at(0);
	if (level->atoms.isEmpty())
		return;
	const ConfigAtom::Ptr atom = level->atoms.value(0);
	Q_ASSERT(!atom || level->arrayElement || !atom->typeMap);
	if (level->arrayElement) {
		Q_ASSERT(d->levels.size() > 2);
		/*delete*/ d->levels.takeFirst();
		/*delete*/ d->levels.takeFirst();
	} else if (!atom->typeMap) {
		Q_ASSERT(d->levels.size() > 1);
		/*delete*/ d->levels.takeFirst();
	}
}

int Config::arraySize() const
{
	Q_D(const Config);
	const ConfigLevel::Ptr &level = d->levels.at(0);
	ConfigAtom::Ptr  const atom = level->atoms.value(0);
	if (!atom || atom->typeMap) {
		return 0;
	} else {
		int size = atom->list->size();
		if (!size && level->atoms.size() > 1)
			size = level->atoms.at(1)->list->size();
		return size;
	}
}

void Config::setArrayIndex(int index)
{
	Q_D(Config);
	const ConfigLevel::Ptr &level = d->levels.at(0);
	ConfigAtom::Ptr atom = level->atoms.value(0);
	Q_ASSERT(level->arrayElement || !atom || !atom->typeMap);
	if (!atom) {
		if (!level->arrayElement) {
			d->levels.prepend(ConfigLevel::Ptr(new ConfigLevel));
			d->current()->arrayElement = true;
		}
		return;
	}
	if (!level->arrayElement) {
		const ConfigLevel::Ptr l(new ConfigLevel());
		l->arrayElement = true;
		d->levels.prepend(l);
	}
	const ConfigLevel::Ptr &l = d->levels.at(1);
	QList<ConfigAtom::Ptr> &atoms = l->atoms;
	//		qDeleteAll(d->current()->atoms);
	d->current()->atoms.clear();
	for (int i = 0; i < atoms.size(); i++) {
		atom = atoms.at(i);
		if (atom->readOnly && !atom->typeMap && atom->list->size() > index) {
			QVariant &var = (*(atom->list))[index];
			if (var.type() == QVariant::Map)
				d->current()->atoms << ConfigAtom::Ptr(new ConfigAtom(var, true));
		} else if (!atom->readOnly && !atom->typeMap) {
#ifndef CONFIG_MAKE_DIRTY_ONLY_AT_SET_VALUE
			bool *changed = !d->sources.isEmpty() ? &d->sources.at(0)->dirty : 0;
#endif
			while (atom->list->size() <= index) {
#ifndef CONFIG_MAKE_DIRTY_ONLY_AT_SET_VALUE
				if (changed && !(*changed))
					d->sources.at(0)->makeDirty();
#endif
				atom->list->append(QVariantMap());
			}
			QVariant &var = (*(atom->list))[index];
#ifndef CONFIG_MAKE_DIRTY_ONLY_AT_SET_VALUE
			if (changed && !(*changed) && var.type() == QVariant::Map)
				d->sources.at(0)->makeDirty();
#endif
			d->current()->atoms << ConfigAtom::Ptr(new ConfigAtom(var, true));
		}
	}
}

void Config::remove(int index)
{
	Q_D(Config);
	ConfigLevel::Ptr level = d->levels.at(0);
	if (level->arrayElement) {
		Q_ASSERT(d->levels.size() > 1);
		/*delete*/ d->levels.takeFirst();
		level = d->levels.at(0);
	}
	ConfigAtom::Ptr atom = level->atoms.value(0);
	Q_ASSERT(!atom || !atom->typeMap);
	if (atom && !atom->readOnly && atom->list->size() > index) {
		atom->list->removeAt(index);
		if (!d->sources.isEmpty())
			d->sources.at(0)->makeDirty();
	}
}

QVariant Config::rootValue(const QVariant &def, ValueFlags type) const
{
	Q_D(const Config);
	if (d->levels.at(0)->atoms.isEmpty())
		return def;
	const ConfigAtom::Ptr &atom = d->levels.at(0)->atoms.first();
	QVariant var = atom->typeMap ? QVariant(*atom->map) : QVariant(*atom->list);
	if (type & Config::Crypted)
		return var.isNull() ? def : CryptoService::decrypt(var);
	else
		return var.isNull() ? def : var;
}

QVariant Config::value(const QString &key, const QVariant &def, ValueFlags type) const
{
	Q_D(const Config);
	if (d->levels.at(0)->atoms.isEmpty())
		return def;
	QString name = key;
	int slashIndex = name.lastIndexOf('/');
	if (slashIndex != -1) {
		const_cast<Config*>(this)->beginGroup(name.mid(0, slashIndex));
		name = name.mid(slashIndex + 1);
	}
	const ConfigLevel::Ptr &level = d->current();
	QVariant var;
	QList<ConfigAtom::Ptr> &atoms = level->atoms;
	for (int i = 0; i < atoms.size(); i++) {
		ConfigAtom::Ptr atom = level->atoms.at(i);
		Q_ASSERT(atom->typeMap);
		var = atom->map->value(name);
		if (!var.isNull())
			break;
	}
	if (slashIndex != -1)
		const_cast<Config*>(this)->endGroup();
	if (type & Config::Crypted)
		return var.isNull() ? def : CryptoService::decrypt(var);
	else
		return var.isNull() ? def : var;
}

void Config::setValue(const QString &key, const QVariant &value, ValueFlags type)
{
	Q_D(Config);
	if (d->levels.at(0)->atoms.isEmpty())
		return;
	QString name = key;
	int slashIndex = name.lastIndexOf('/');
	if (slashIndex != -1) {
		beginGroup(name.mid(0, slashIndex));
		name = name.mid(slashIndex + 1);
	}
	ConfigAtom::Ptr atom = d->current()->atoms.at(0);
	Q_ASSERT(atom->typeMap);
	QVariant var = (type & Config::Crypted) ? CryptoService::crypt(value) : value;
	QVariant &currentVar = (*atom->map)[name];
	if (var != currentVar) {
		currentVar = var;
		if (!d->sources.isEmpty())
			d->sources.at(0)->makeDirty();
	}
	if (slashIndex != -1)
		endGroup();
}

void Config::sync()
{
	d_func()->sync();
}

void Config::registerType(int type, SaveOperator saveOp, LoadOperator loadOp)
{
	Q_UNUSED(type);
	Q_UNUSED(saveOp);
	Q_UNUSED(loadOp);
}

class ConfigBackendPrivate
{
public:
	mutable QByteArray extension;
};

ConfigBackend::ConfigBackend() : d_ptr(new ConfigBackendPrivate)
{
}

ConfigBackend::~ConfigBackend()
{
}

QByteArray ConfigBackend::name() const
{
	Q_D(const ConfigBackend);
	if(d->extension.isNull()) {
		d->extension = MetaObjectBuilder::info(metaObject(), "Extension");
		d->extension = d->extension.toLower();
	}
	return d->extension;
}

void ConfigBackend::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}
}

