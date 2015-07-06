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
#include <QSharedPointer>
#include <QStringBuilder>
#include <QTimer>
#include <QPointer>

#define CONFIG_MAKE_DIRTY_ONLY_AT_SET_VALUE 1

namespace qutim_sdk_0_3
{
Q_GLOBAL_STATIC(QList<ConfigBackend*>, all_config_backends)
LIBQUTIM_EXPORT QList<ConfigBackend*> &get_config_backends()
{ return *all_config_backends(); }

class ConfigPath
{
public:
	enum SpecialValue { Invalid };

	ConfigPath(const QString &path, const QString &name) : m_frozen(false), m_path(path), m_name(name) {}
	ConfigPath(SpecialValue) : m_frozen(true) {}
	ConfigPath() = delete;

	ConfigPath child(const QString &name) const
	{
		if (m_frozen)
			return *this;
		return ConfigPath(m_path, m_name % QLatin1Char('/') % name);
	}

	ConfigPath freeze() const
	{
		ConfigPath result = *this;
		result.m_frozen = true;
		return result;
	}

	bool isFrozen() const
	{
		return m_frozen;
	}

	QString path() const
	{
		return m_path;
	}

	QString name() const
	{
		return m_name;
	}

private:
	bool m_frozen;
	QString m_path;
	QString m_name;
};

class ConfigAtom;

class ConfigSource
{
	Q_DISABLE_COPY(ConfigSource)
public:
	typedef QSharedPointer<ConfigSource> Ptr;
	typedef QWeakPointer<ConfigSource> WeakPtr;

	ConfigSource();
	~ConfigSource();

	static ConfigSource::Ptr open(const QString &path, bool systemDir, bool create, ConfigBackend *bcknd = 0);
	inline void update() { lastModified = QFileInfo(fileName).lastModified(); }
	bool isValid() {
		return QFileInfo(fileName).lastModified() == lastModified;
	}
	void sync();
	void makeDirty() { dirty = true; }
	QString fileName;
	ConfigBackend *backend;
	bool dirty;
	bool isAtLoop;
	QSharedPointer<ConfigAtom> data;
	QDateTime lastModified;
};

namespace Detail {

class ConfigConnectionData : public QSharedData
{
	Q_DISABLE_COPY(ConfigConnectionData)
public:
	ConfigConnectionData(const QList<ConfigPath> &paths);
	~ConfigConnectionData();

	void onChange(const QVariant &value);

	const QList<ConfigPath> paths;
	QVector<QPair<qint64, std::function<void (const QVariant &)>>> callbacks;
};

}

class ConfigNotifier
{
public:
	void mark(const ConfigPath &path)
	{
		m_changedPaths.insert(qMakePair(path.path(), path.name()));

		if (!m_eventSent) {
			m_eventSent = true;
			QTimer::singleShot(0, [this] () {
				if (qApp->closingDown())
					return;
				notify();
			});
		}
	}

	void notify();

	void listen(const ConfigPath &path, Detail::ConfigConnectionData *connection)
	{
		auto &connections = m_connections[qMakePair(path.path(), path.name())];
		connections << connection;
	}

	void forget(const ConfigPath &path, Detail::ConfigConnectionData *connection)
	{
		const auto id = qMakePair(path.path(), path.name());
		auto &connections = m_connections[id];
		connections.remove(connection);

		if (connections.isEmpty()) {
			m_connections.remove(id);
		}
	}

	qint64 nextListenerId()
	{
		return ++m_listenerIdCounter;
	}

private:
	QSet<QPair<QString, QString>> m_changedPaths;
	QHash<QPair<QString, QString>, QSet<Detail::ConfigConnectionData *>> m_connections;
	qint64 m_listenerIdCounter = 0;
	bool m_eventSent = false;
};

Q_GLOBAL_STATIC(ConfigNotifier, config_notifier)

Detail::ConfigConnectionData::ConfigConnectionData(const QList<ConfigPath> &paths) : QSharedData(), paths(paths)
{
	for (const ConfigPath &path : paths)
		config_notifier()->listen(path, this);
}

Detail::ConfigConnectionData::~ConfigConnectionData()
{
	for (const ConfigPath &path : paths)
		config_notifier()->forget(path, this);
}

void Detail::ConfigConnectionData::onChange(const QVariant &value)
{
	for (const auto &pair : callbacks)
		pair.second(value);
}

Detail::ConfigConnection::ConfigConnection()
{
}

Detail::ConfigConnection::ConfigConnection(const Detail::ConfigConnection &other) : QSharedData(other)
{
}

Detail::ConfigConnection::~ConfigConnection()
{
}

Detail::ConfigConnection &Detail::ConfigConnection::operator =(const Detail::ConfigConnection &other)
{
	m_data = other.m_data;
	return *this;
}

qint64 Detail::ConfigConnection::onChange(const std::function<void (const QVariant &)> &callback)
{
	if (Q_UNLIKELY(!m_data)) {
		qFatal("Detail::ConfigConnection::onChange: m_data is null");
	}
	qint64 listenerId = config_notifier()->nextListenerId();
	m_data->callbacks << qMakePair(listenerId, callback);
	return listenerId;
}

void Detail::ConfigConnection::forget(qint64 listenerId)
{
	if (Q_UNLIKELY(!m_data)) {
		qFatal("Detail::ConfigConnection::onChange: m_data is null");
	}
	for (int i = m_data->callbacks.size() - 1; i >= 0; --i) {
		if (m_data->callbacks[i].first == listenerId)
			m_data->callbacks.remove(i);
	}
}

class ConfigAtom
{
public:
	typedef QSharedPointer<ConfigAtom> Ptr;
	typedef QMap<QString, Ptr> ConfigMap;
	typedef QVector<Ptr> ConfigList;
	typedef QVariant ConfigValue;

	enum Type {
		List,
		Map,
		Value,
		Null
	};

	ConfigAtom(const ConfigSource::WeakPtr &source, bool readOnly, const ConfigPath &path)
		: m_source(source), m_path(path), m_type(Null), m_readOnly(readOnly)
	{
	}

	~ConfigAtom()
	{
		clear();
	}

	ConfigAtom(const ConfigAtom &other) = delete;
	ConfigAtom &operator =(const ConfigAtom &other) = delete;

	static Ptr fromVariant(const ConfigSource::WeakPtr &source, const QVariant &variant, bool readOnly, const ConfigPath &path)
	{
		auto result = Ptr::create(source, readOnly, path);

		// Null value
		if (!variant.isValid())
			return result;

		switch (variant.type()) {
		case QVariant::Map: {
			const auto &input = variant.toMap();
			auto &map = result->ensureMap();
			for (auto it = input.begin(); it != input.end(); ++it)
				map.insert(it.key(), fromVariant(source, it.value(), readOnly, path.child(it.key())));
		}
			break;
		case QVariant::List: {
			const auto &input = variant.toList();
			auto &list = result->ensureList();
			list.reserve(input.size());
			for (const auto &value : input)
				list.append(fromVariant(source, value, readOnly, path.freeze()));
		}
			break;
		default:
			result->ensureValue() = variant;
			break;
		}

		return result;
	}

	bool isReadOnly() const
	{
		return m_readOnly;
	}

	bool isMap() const
	{
		return m_type == Map;
	}

	bool isList() const
	{
		return m_type == List;
	}

	bool isValue() const
	{
		return m_type == Value;
	}

	bool isNull() const
	{
		return m_type == Null;
	}

	Type type() const
	{
		return m_type;
	}

	QVariant toVariant()
	{
		switch (m_type) {
		case Map: {
			QVariantMap result;
			const auto &map = asMap();
			for (auto it = map.begin(); it != map.end(); ++it)
				result.insert(it.key(), it.value()->toVariant());
			return result;
		}
		case List: {
			QVariantList result;
			const auto &list = asList();
			for (const auto &value : list)
				result.append(value->toVariant());
			return result;
		}
		case Value:
			return asValue();
		case Null:
			return QVariant();
		}
		return QVariant();
	}

	Ptr child(const QString &name)
	{
		Q_ASSERT(!m_readOnly || isMap());
		auto &map = ensureMap();

		auto it = map.find(name);
		if (it == map.end()) {
			if (m_readOnly)
				return Ptr();

			it = map.insert(name, Ptr::create(m_source, m_readOnly, m_path.child(name)));
		}

		return it.value();
	}

	Ptr child(int index)
	{
		Q_ASSERT(!m_readOnly || isList());
		auto &list = ensureList();

		if (m_readOnly && list.size() <= index)
			return Ptr();

		while (list.size() <= index)
			list.append(Ptr::create(m_source, m_readOnly, m_path.freeze()));

		return list.at(index);
	}

	int arraySize()
	{
		Q_ASSERT(isList());
		auto &list = asList();
		return list.size();
	}

	template <typename Callback>
	void iterateChildren(Callback callback)
	{
		if (isMap()) {
			for (auto value : asMap())
				callback(value);
		} else if (isList()) {
			for (auto value : asList())
				callback(value);
		}
	}

	template <typename Callback>
	void iterateMap(Callback callback)
	{
		Q_ASSERT(isMap());

		auto &map = asMap();
		for (auto it = map.begin(); it != map.end(); ++it)
			callback(it.key(), it.value());
	}

	void remove(int index)
	{
		Q_ASSERT(isList());
		auto &list = asList();

		if (index < list.size()) {
			list.remove(index);
			makeDirty();
		}
	}

	void remove(const QString &name)
	{
		Q_ASSERT(isMap());
		if (Ptr atom = asMap().take(name)) {
			atom->markMeAndChildren();
			makeDirty();
		}
	}

	void replace(const QVariant &value)
	{
		Q_ASSERT(!m_readOnly);

		if (toVariant() != value) {
			ConfigAtom::Ptr other = ConfigAtom::fromVariant(m_source, value, m_readOnly, m_path);
			markMeAndChildren();

			switch (other->type()) {
			case Map:
				ensureMap() = other->asMap();
				break;
			case List:
				ensureList() = other->asList();
				break;
			case Value:
				ensureValue() = other->asValue();
				break;
			case Null:
				clear();
				break;
			}

			makeDirty();
		}
	}

	void convert(Type type)
	{
		Q_ASSERT(!m_readOnly);
		if (m_type == type)
			return;

		switch (type) {
		case Map:
			ensureMap();
			break;
		case List:
			ensureList();
			break;
		case Value:
			ensureValue();
			break;
		case Null:
			clear();
			break;
		}
	}

	const ConfigPath &path() const
	{
		return m_path;
	}

private:
	void makeDirty()
	{
		markMeAndChildren();
		if (auto source = m_source.toStrongRef())
			source->makeDirty();
	}

	void markMeAndChildren()
	{
		mark();
		if (isMap() && !m_path.isFrozen()) {
			iterateMap([] (const QString &key, const Ptr &child) {
				Q_UNUSED(key);
				child->markMeAndChildren();
			});
		}
	}

	void mark() const
	{
		config_notifier()->mark(m_path);
	}

	ConfigMap &asMap()
	{
		Q_ASSERT(isMap());
		return ensureMap();
	}

	ConfigList &asList()
	{
		Q_ASSERT(isList());
		return ensureList();
	}

	ConfigValue &asValue()
	{
		Q_ASSERT(isValue());
		return ensureValue();
	}

	ConfigMap &ensureMap()
	{
		auto map = data<ConfigMap>();

		if (m_type != Map) {
			clear();
			new (map) ConfigMap();
			m_type = Map;
		}

		return *map;
	}

	ConfigList &ensureList()
	{
		auto list = data<ConfigList>();

		if (m_type != List) {
			clear();
			new (list) ConfigList();
			m_type = List;
		}

		return *list;
	}

	ConfigValue &ensureValue()
	{
		auto value = data<ConfigValue>();

		if (m_type != Value) {
			clear();
			new (value) ConfigValue();
			m_type = Value;
		}

		return *value;
	}

private:
	void clear()
	{
		switch (m_type) {
		case Map:
			data<ConfigMap>()->~ConfigMap();
			break;
		case List:
			data<ConfigList>()->~ConfigList();
			break;
		case Value:
			data<ConfigValue>()->~ConfigValue();
			break;
		case Null:
			break;
		}

		m_type = Null;
	}

	template <typename T>
	T *data()
	{
		char *buffer = m_buffer;
		return reinterpret_cast<T *>(buffer);
	}

	ConfigSource::WeakPtr m_source;
	ConfigPath m_path;
	union {
		char m_map_buffer[sizeof(ConfigMap)];
		char m_list_buffer[sizeof(ConfigList)];
		char m_value_buffer[sizeof(ConfigValue)];
		char m_buffer[1];
	};
	Type m_type;
	const bool m_readOnly;
};

class ConfigSourceHash : public QObject
{
public:
	ConfigSource::Ptr value(const QString &key)
	{
		auto it = m_hash.find(key);
		if (it == m_hash.end())
			return ConfigSource::Ptr();

		m_timers.remove(it->timer.timerId());
		it->timer.start(5 * 60, this);
		m_timers.insert(it->timer.timerId(), key);

		return it->config;
	}

	void insert(const QString &key, const ConfigSource::Ptr &value)
	{
		auto it = m_hash.find(key);
		if (it == m_hash.end())
			it = m_hash.insert(key, Info());

		m_timers.remove(it->timer.timerId());
		it->timer.start(5 * 60, this);
		m_timers.insert(it->timer.timerId(), key);

		it->config = value;
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

void ConfigNotifier::notify()
{
	m_eventSent = false;
	auto changedPaths = m_changedPaths;
	m_changedPaths.clear();
	foreach (auto pair, changedPaths) {
		QString path = pair.second;
		int index;
		while ((index = path.lastIndexOf(QLatin1Char('/'))) >= 0) {
			path.resize(index);
			changedPaths.insert(qMakePair(pair.first, path));
		}
	}
	auto sortedPaths = changedPaths.toList();
	typedef const QPair<QString, QString> & Pair;
	std::sort(sortedPaths.begin(), sortedPaths.end(), [] (Pair first, Pair second) {
		return std::make_tuple(first.first, -first.second.size(), first.second)
				< std::make_tuple(second.first, -second.second.size(), second.second);
	});
	foreach (Pair path, sortedPaths) {
		auto it = m_connections.find(path);
		if (it == m_connections.end())
			continue;

		QVariant value = Config(path.first).value(path.second);
		foreach (Detail::ConfigConnectionData *connection, it.value())
			connection->onChange(value);
	}
}

ConfigSource::ConfigSource() : backend(nullptr), dirty(false), isAtLoop(false)
{
}

ConfigSource::~ConfigSource()
{
	if (dirty)
		sync();
}

ConfigSource::Ptr ConfigSource::open(const QString &path, bool systemDir, bool create, ConfigBackend *backend)
{
	QString fileName = path;
	if (fileName.isEmpty())
		fileName = QLatin1String("profile");
	QFileInfo info(fileName);
	QString originalPath = info.filePath();
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
			backend = backends.first();
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

	result = ConfigSource::Ptr::create();

	ConfigSource *d = result.data();
	d->backend = backend;
	d->fileName = fileName;
	// QFileInfo says that we can't write to non-exist files but we can
	const bool readOnly = !info.isWritable() && (systemDir || info.exists());

	d->update();
	const QVariant value = d->backend->load(d->fileName);
	ConfigPath configPath = readOnly ? ConfigPath(ConfigPath::Invalid) : ConfigPath(originalPath, QString());
	d->data = ConfigAtom::fromVariant(result, value, readOnly, configPath);

	if (d->data->isValue() || d->data->isNull()) {
		if (!create)
			return ConfigSource::Ptr();

		d->data = ConfigAtom::fromVariant(result, QVariantMap(), readOnly, configPath);
	}

	sourceHash()->insert(fileName, result);
	return result;
}

void ConfigSource::sync()
{
	backend->save(fileName, data->toVariant());
	dirty = false;
	update();
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

class ConfigLevel
{
public:
	typedef QSharedPointer<ConfigLevel> Ptr;

	explicit ConfigLevel(const QList<ConfigAtom::Ptr> &atoms)
		: atoms(atoms), arrayElement(false)
	{
	}
	~ConfigLevel()
	{
	}

	ConfigLevel::Ptr child(int index);
	ConfigLevel::Ptr child(const QString &name);
	ConfigLevel::Ptr child(const QStringList &names);

	template <typename Callback>
	void iterateChildren(Callback callback);
	template <typename Callback>
	void iterateMap(Callback callback);

	ConfigLevel::Ptr convert(ConfigAtom::Type type);

	QList<ConfigAtom::Ptr> atoms;
	bool arrayElement;

private:
	template <typename Callback>
	ConfigLevel::Ptr map(Callback callback);
};

ConfigLevel::Ptr ConfigLevel::child(int index)
{
	return map([index] (const ConfigAtom::Ptr &atom, bool readOnly) {
		if (readOnly && !atom->isList())
			return ConfigAtom::Ptr();
		return atom->child(index);
	});
}

ConfigLevel::Ptr ConfigLevel::child(const QString &name)
{
	return map([name] (const ConfigAtom::Ptr &atom, bool readOnly) {
		if (readOnly && !atom->isMap())
			return ConfigAtom::Ptr();
		return atom->child(name);
	});
}

ConfigLevel::Ptr ConfigLevel::child(const QStringList &names)
{
	Q_ASSERT(!names.isEmpty());

	ConfigLevel::Ptr level = child(names.first());
	for (int i = 1; i < names.size(); ++i)
		level = level->child(names[i]);

	return level;
}

template <typename Callback>
void ConfigLevel::iterateChildren(Callback callback)
{
	for (const ConfigAtom::Ptr &atom : atoms) {
		atom->iterateChildren(callback);
	}
}

template <typename Callback>
void ConfigLevel::iterateMap(Callback callback)
{
	for (const ConfigAtom::Ptr &atom : atoms) {
		if (atom->isMap())
			atom->iterateMap(callback);
	}
}

ConfigLevel::Ptr ConfigLevel::convert(ConfigAtom::Type type)
{
	return map([type] (const ConfigAtom::Ptr &atom, bool readOnly) -> ConfigAtom::Ptr {
		if (readOnly && atom->type() != type)
			return ConfigAtom::Ptr();

		if (atom->type() != type)
			atom->convert(type);

		return atom;
	});
}

template <typename Callback>
ConfigLevel::Ptr ConfigLevel::map(Callback callback)
{
	bool first = true;
	QList<ConfigAtom::Ptr> results;

	for (const ConfigAtom::Ptr &atom : atoms) {
		const bool isReadOnly = atom->isReadOnly() || !first;
		first = false;

		if (auto result = callback(atom, isReadOnly))
			results << result;
	}

	return ConfigLevel::Ptr::create(results);
}

class ConfigPrivate : public QSharedData
{
	Q_DISABLE_COPY(ConfigPrivate)
public:
	ConfigPrivate();
	ConfigPrivate(const QStringList &paths, ConfigBackend *backend = 0);
	ConfigPrivate(const QStringList &paths, const QVariantList &fallbacks, ConfigBackend *backend = nullptr);
	~ConfigPrivate();

	inline const ConfigLevel::Ptr &current() const { return levels.first(); }

	void sync();
	QExplicitlySharedDataPointer<ConfigPrivate> clone();

	QList<ConfigLevel::Ptr> levels;
	QList<ConfigSource::Ptr> sources;

	QExplicitlySharedDataPointer<ConfigPrivate> memoryGuard;
};

ConfigPrivate::ConfigPrivate()
{
	levels << ConfigLevel::Ptr::create(QList<ConfigAtom::Ptr>());
}

ConfigPrivate::ConfigPrivate(const QStringList &paths, ConfigBackend *backend)
	: ConfigPrivate(paths, QVariantList(), backend)
{
}

ConfigPrivate::ConfigPrivate(const QStringList &paths, const QVariantList &fallbacks, ConfigBackend *backend)
	: ConfigPrivate()
{
	QSet<QString> opened;
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < paths.size(); i++) {
			// Firstly we should open user-specific configs
			ConfigSource::Ptr source = ConfigSource::open(paths.at(i), j == 1, sources.isEmpty(), backend);
			if (source && !opened.contains(source->fileName)) {
				opened.insert(source->fileName);
				sources << source;
			}
		}
	}
	for (int i = 0; i < sources.size(); i++) {
		ConfigSource::Ptr source = sources.at(i);
		current()->atoms << source->data;
	}
	for (int i = 0; i < fallbacks.size(); ++i) {
		const QVariant value = fallbacks.at(i);
		auto fallback = ConfigAtom::fromVariant(ConfigSource::Ptr(), value, true, ConfigPath::Invalid);

		if (fallback->isNull() || fallback->isValue())
			continue;

		current()->atoms << fallback;
	}
}

ConfigPrivate::~ConfigPrivate()
{
	if (!memoryGuard)
		sync();
}

void ConfigPrivate::sync()
{
	if (sources.isEmpty())
		return;

	ConfigSource::Ptr source = sources.value(0);
	if (source && source->dirty && !source->isAtLoop) {
		source->isAtLoop = true;
		source->dirty = false;
		QCoreApplication::postEvent(postConfigSaver(), new PostConfigSaveEvent(source), -2);
	}
}

QExplicitlySharedDataPointer<ConfigPrivate> ConfigPrivate::clone()
{
	QExplicitlySharedDataPointer<ConfigPrivate> result(new ConfigPrivate);

	*result->current() = *current();
	result->sources = sources;

	return result;
}

Config::Config(const QVariantList &list) : d_ptr(new ConfigPrivate)
{
	Q_D(Config);
	d->current()->atoms << ConfigAtom::fromVariant(ConfigSource::Ptr(), list, false, ConfigPath::Invalid);
}

Config::Config(const QVariantMap &map) : d_ptr(new ConfigPrivate)
{
	Q_D(Config);
	d->current()->atoms << ConfigAtom::fromVariant(ConfigSource::Ptr(), map, false, ConfigPath::Invalid);
}

Config::Config(const QString &path)
	: d_ptr(new ConfigPrivate(QStringList(path)))
{
}

Config::Config(const QString &path, ConfigBackend *backend)
	: d_ptr(new ConfigPrivate(QStringList(path), backend))
{
}

Config::Config(const QStringList &paths)
	: d_ptr(new ConfigPrivate(paths))
{
}

Config::Config(const QString &path, const QVariantList &fallbacks)
	: d_ptr(new ConfigPrivate(QStringList(path), fallbacks))
{
}

Config::Config(const QString &path, const QVariant &fallback)
	: d_ptr(new ConfigPrivate(QStringList(path), QVariantList() << fallback))
{
}

Config::Config(const QExplicitlySharedDataPointer<ConfigPrivate> &d) : d_ptr(d)
{
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

	beginGroup(fullName);
	auto p = d->clone();
	p->memoryGuard = d_ptr;
	endGroup();

	return Config(p);
}

QStringList Config::childGroups() const
{
	Q_D(const Config);
	QStringList groups;

	d->current()->iterateMap([&groups] (const QString &name, const ConfigAtom::Ptr &atom) {
		if (atom->isMap() && !groups.contains(name))
			groups << name;
	});

	return groups;
}

QStringList Config::childKeys() const
{
	Q_D(const Config);
	QStringList keys;

	d->current()->iterateMap([&keys] (const QString &name, const ConfigAtom::Ptr &atom) {
		if (!atom->isMap() && !keys.contains(name))
			keys << name;
	});

	return keys;
}

bool Config::hasChildGroup(const QString &name) const
{
	Q_D(const Config);
	bool found = false;

	d->current()->iterateMap([&found, name] (const QString &keyName, const ConfigAtom::Ptr &atom) {
		if (atom->isMap() && keyName == name)
			found = true;
	});

	return found;
}

bool Config::hasChildKey(const QString &name) const
{
	Q_D(const Config);
	bool found = false;

	d->current()->iterateMap([&found, name] (const QString &keyName, const ConfigAtom::Ptr &atom) {
		if (!atom->isMap() && keyName == name)
			found = true;
	});

	return found;
}

static QStringList parseNames(const QString &fullName)
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

	const QStringList names = parseNames(fullName);
	Q_ASSERT(!names.isEmpty());

	d->levels.prepend(d->current()->child(names)->convert(ConfigAtom::Map));
}

void Config::endGroup()
{
	Q_D(Config);
	Q_ASSERT(d->levels.size() > 1);
	d->levels.takeFirst();
}

void Config::remove(const QString &name)
{
	Q_D(Config);
	d->current()->atoms.first()->remove(name);
}

Config Config::arrayElement(int index)
{
	Q_D(Config);

	auto p = d->clone();
	p->memoryGuard = d_ptr;

	Config cfg(p);
	cfg.setArrayIndex(index);
	return cfg;
}

int Config::beginArray(const QString &name)
{
	Q_D(Config);
	Q_ASSERT(!name.isEmpty());

	const QStringList names = parseNames(name);
	d->levels.prepend(d->current()->child(names)->convert(ConfigAtom::List));

	return arraySize();
}

void Config::endArray()
{
	Q_D(Config);
	Q_ASSERT(d->levels.size() > 1);

	if (d->current()->arrayElement)
		d->levels.takeFirst();

	Q_ASSERT(d->levels.size() > 1);
	Q_ASSERT(!d->current()->arrayElement);
	d->levels.takeFirst();
}

int Config::arraySize() const
{
	Q_D(const Config);

	const ConfigLevel::Ptr level = d->current()->arrayElement ? d->levels.at(1) : d->current();

	int size = 0;
	for (ConfigAtom::Ptr atom : level->atoms) {
		if ((size = atom->arraySize()) > 0)
			break;
	}

	return size;
}

void Config::setArrayIndex(int index)
{
	Q_D(Config);

	if (d->current()->arrayElement)
		d->levels.takeFirst();

	const ConfigLevel::Ptr level = d->current();
	Q_ASSERT(level->atoms.first()->isList());

	const ConfigLevel::Ptr arrayElement = level->child(index)->convert(ConfigAtom::Map);
	arrayElement->arrayElement = true;
	d->levels.prepend(arrayElement);
}

void Config::remove(int index)
{
	Q_D(Config);

	if (d->current()->arrayElement)
		d->levels.takeFirst();

	d->current()->atoms.first()->remove(index);
}

QVariant Config::rootValue(const QVariant &def, ValueFlags type) const
{
	Q_D(const Config);

	if (d->current()->atoms.isEmpty())
		return def;

	QVariant var = d->current()->atoms.first()->toVariant();
	if (type & Config::Crypted)
		return var.isNull() ? def : CryptoService::decrypt(var);
	else
		return var.isNull() ? def : var;
}

ConfigValue<QVariant> Config::value(const QString &key, const QVariant &def, ValueFlags type) const
{
	Q_D(const Config);
	Q_UNUSED(type);

	typedef QExplicitlySharedDataPointer<Detail::ConfigConnectionData> ConfigConnectionDataPtr;
	typedef Detail::ConfigValueData<QVariant> ConfigValueData;
	typedef typename ConfigValueData::Ptr ConfigValueDataPtr;

	Detail::ConfigConnection connection;
	if (d->current()->atoms.isEmpty()) {
		connection.m_data = ConfigConnectionDataPtr(new Detail::ConfigConnectionData(QList<ConfigPath>()));
		return ConfigValue<QVariant>(ConfigValueDataPtr(new ConfigValueData(def, connection)));
	}

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
		Q_ASSERT(atom->isMap());

		ConfigAtom::Ptr child = atom->child(name);
		if (child && !child->isNull()) {
			var = child->toVariant();
		}

		if (!var.isNull())
			break;
	}

	QList<ConfigPath> paths;
	for (int i = 0; i < atoms.size(); i++) {
		ConfigAtom::Ptr atom = level->atoms.at(i);
		Q_ASSERT(atom->isMap());

		if (!atom->isReadOnly()) {
			paths << atom->path();
		}
	}

	if (slashIndex != -1)
		const_cast<Config*>(this)->endGroup();

	typedef QExplicitlySharedDataPointer<Detail::ConfigConnectionData> ConfigConnectionDataPtr;
	connection.m_data = ConfigConnectionDataPtr(new Detail::ConfigConnectionData(paths));
	ConfigValueDataPtr data(new ConfigValueData(var.isNull() ? def : var, connection));
	return ConfigValue<QVariant>(data);
}

void Config::setValue(const QString &key, const QVariant &value, ValueFlags type)
{
	Q_D(Config);
	if (d->current()->atoms.isEmpty())
		return;

	QString name = key;
	int slashIndex = name.lastIndexOf('/');
	if (slashIndex != -1) {
		beginGroup(name.mid(0, slashIndex));
		name = name.mid(slashIndex + 1);
	}

	QVariant var = (type & Config::Crypted) ? CryptoService::crypt(value) : value;

	ConfigAtom::Ptr atom = d->current()->atoms.first();
	Q_ASSERT(atom->isMap() && !atom->isReadOnly());
	atom->child(name)->replace(var);

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

