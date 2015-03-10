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

#ifndef CONFIG_H
#define CONFIG_H

#include "libqutim_global.h"
#include <QVariant>
#include <QSharedData>
#include <QMetaTypeId>

namespace qutim_sdk_0_3
{
#ifndef Q_QDOC
namespace Detail
{

template <typename T, bool is_enum>
struct ConfigValueCasterHelper;

template <typename T>
struct ConfigValueCasterHelper<T, false>
{
	static QVariant toVariant(const T &t)
	{
		return QVariant::fromValue<T>(t);
	}

	static T fromVariant(const QVariant &v)
	{
		return v.value<T>();
	}
};

template <typename T>
struct ConfigValueCasterHelper<T, true>
{
	typedef typename std::underlying_type<T>::type Type;

	static QVariant toVariant(const T &t)
	{
		return QVariant::fromValue<Type>(static_cast<Type>(t));
	}

	static T fromVariant(const QVariant &v)
	{
		return static_cast<T>(v.value<Type>());
	}
};

template <typename T>
struct ConfigValueCaster : public ConfigValueCasterHelper<T, std::is_enum<T>::value>
{
};

template <typename T>
struct ConfigValueCaster<QFlags<T>>
{
	typedef typename std::underlying_type<T>::type Type;
	typedef ConfigValueCaster<Type> Caster;

	static QVariant toVariant(const QFlags<T> &t)
	{
		return Caster::toVariant(static_cast<Type>(t));
	}

	static QFlags<T> fromVariant(const QVariant &v)
	{
		return static_cast<QFlags<T>>(Caster::fromVariant(v));
	}
};

}
#endif
class ConfigPrivate;
class ConfigBackend;
class ConfigBackendPrivate;

class LIBQUTIM_EXPORT Config
{
	Q_DECLARE_PRIVATE(Config)
public:
	enum ValueFlag { Normal = 0x00, Crypted = 0x01 };
	Q_DECLARE_FLAGS(ValueFlags, ValueFlag)

	Config(const QVariantList &list);
	Config(QVariantList *list);
	Config(const QVariantMap &map);
	Config(QVariantMap *map);
	Config(const QString &path = QString());
	Config(const QString &path, ConfigBackend *backend);
	Config(const QStringList &paths);
	Config(const QString &path, const QVariantList &fallbacks);
	Config(const QString &path, const QVariant &fallback);
	Config(const Config &other);
	Config &operator =(const Config &other);
	virtual ~Config();

	Config group(const QString &name) Q_REQUIRED_RESULT;
	QStringList childGroups() const Q_REQUIRED_RESULT;
	QStringList childKeys() const Q_REQUIRED_RESULT;
	bool hasChildGroup(const QString &name) const Q_REQUIRED_RESULT;
	bool hasChildKey(const QString &name) const Q_REQUIRED_RESULT;
	void beginGroup(const QString &name);
	void endGroup();
	void remove(const QString &name);

	Config arrayElement(int index) Q_REQUIRED_RESULT;
	int beginArray(const QString &name);
	void endArray();
	int arraySize() const Q_REQUIRED_RESULT;
	void setArrayIndex(int index);
	void remove(int index);

	QVariant rootValue(const QVariant &def = QVariant(), ValueFlags type = Normal) const Q_REQUIRED_RESULT;
	template<typename T>
	T value(const QString &key, const T &def = T(), ValueFlags type = Normal) const Q_REQUIRED_RESULT;
	QVariant value(const QString &key, const QVariant &def = QVariant(), ValueFlags type = Normal) const Q_REQUIRED_RESULT;
	inline QString value(const QString &key, const QLatin1String &def, ValueFlags type = Normal) const Q_REQUIRED_RESULT;
	inline QString value(const QString &key, const char *def, ValueFlags type = Normal) const Q_REQUIRED_RESULT;
	template <int N>
	QString value(const QString &key, const char (&def)[N], ValueFlags type = Normal) const Q_REQUIRED_RESULT;
	template<typename T>
	void setValue(const QString &key, const T &value, ValueFlags type = Normal);
	void setValue(const QString &key, const QVariant &value, ValueFlags type = Normal);
	inline void setValue(const QString &key, const QLatin1String &value, ValueFlags type = Normal);
	inline void setValue(const QString &key, const char *value, ValueFlags type = Normal);
	template <int N>
	void setValue(const QString &key, const char (&value)[N], ValueFlags type = Normal);

	void sync();

	typedef void (*SaveOperator)(QVariant &, const void *);
	typedef void (*LoadOperator)(const QVariant &, void *);
	static void registerType(int type, SaveOperator saveOp, LoadOperator loadOp);
private:
	QExplicitlySharedDataPointer<ConfigPrivate> d_ptr;
};

template <typename T>
void configSaveHelper(QVariant &var, const T *t)
{
    var = QVariant::fromValue(t);
}

template <typename T>
void configLoadHelper(const QVariant &var, T *t)
{
	*t = var.value<T>();
}

template <typename T>
void registerConfigType(T * /* dummy */ = 0)
{
	typedef void (*SavePtr)(QVariant &, const T *);
	typedef void (*LoadPtr)(const QVariant &, T *);
	SavePtr sptr = configSaveHelper<T>();
	LoadPtr lptr = configLoadHelper<T>();

	Config::registerType(qRegisterMetaType<T>(),
						 reinterpret_cast<Config::SaveOperator>(sptr),
						 reinterpret_cast<Config::LoadOperator>(lptr));
}

typedef Config ConfigBase;
typedef Config ConfigGroup;

class LIBQUTIM_EXPORT ConfigBackend : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(ConfigBackend)
public:
	ConfigBackend();
	virtual ~ConfigBackend();

	virtual QVariant load(const QString &file) = 0;
	virtual void save(const QString &file, const QVariant &entry) = 0;

	QByteArray name() const;
protected:
	virtual void virtual_hook(int id, void *data);
private:
	QScopedPointer<ConfigBackendPrivate> d_ptr;
};

template<typename T>
Q_INLINE_TEMPLATE T Config::value(const QString &key, const T &def, Config::ValueFlags type) const
{
	typedef Detail::ConfigValueCaster<T> Caster;
	return Caster::fromVariant(value(key, Caster::toVariant(def), type));
}

template<typename T>
Q_INLINE_TEMPLATE void Config::setValue(const QString &key, const T &value, Config::ValueFlags type)
{
	setValue(key, Detail::ConfigValueCaster<T>::toVariant(value), type);
}

QString Config::value(const QString &key, const QLatin1String &def, ValueFlags type) const
{
	return value(key, QString(def), type);
}

QString Config::value(const QString &key, const char *def, ValueFlags type) const
{
	return value(key, QString::fromUtf8(def), type);
}

template <int N>
Q_INLINE_TEMPLATE QString Config::value(const QString &key, const char (&def)[N], ValueFlags type) const
{
	return value(key, QString::fromUtf8(def, N-1), type);
}

void Config::setValue(const QString &key, const QLatin1String &value, ValueFlags type)
{
	setValue(key, QString(value), type);
}

void Config::setValue(const QString &key, const char *value, ValueFlags type)
{
	setValue(key, QString::fromUtf8(value), type);
}

template <int N>
Q_INLINE_TEMPLATE void Config::setValue(const QString &key, const char (&value)[N], ValueFlags type)
{
	setValue(key, QString::fromUtf8(value, N-1), type);
}
}

//// Config() is synonym for Config("profile"), so redefine construct method for it
//template <>
//Q_INLINE_TEMPLATE void *qMetaTypeConstructHelper<qutim_sdk_0_3::Config>(const qutim_sdk_0_3::Config *t)
//{
//if (!t) {
//    return new qutim_sdk_0_3::Config(QVariantMap());
//}
//return new qutim_sdk_0_3::Config(*t);
//}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Config)

#endif // CONFIG_H

