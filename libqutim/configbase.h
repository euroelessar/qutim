#ifndef CONFIGBASE_H
#define CONFIGBASE_H

#include "libqutim_global.h"
#include <QVariant>
#include <QSharedPointer>

namespace qutim_sdk_0_3
{
	class ConfigGroup;
	class ConfigBasePrivate;
	class ConfigGroupPrivate;
	class ConfigPrivate;

	class LIBQUTIM_EXPORT ConfigBase
	{
	public:
		QStringList groupList() const;
		bool hasGroup(const QString &group) const;
		ConfigGroup group(const QString &group);
		const ConfigGroup group(const QString &group) const;
		inline  const ConfigGroup constGroup(const QString &group) const;
		void removeGroup(const QString &name);

		virtual void sync() = 0;
	protected:
		ConfigBase();
		virtual ~ConfigBase();
		bool isGroup() const;
		virtual void virtual_hook(int, void*) {}
	private:
		QExplicitlySharedDataPointer<ConfigBasePrivate> get_p() const;
	};

	class LIBQUTIM_EXPORT Config : public ConfigBase
	{
	public:
		enum ValueFlag { Normal = 0x00, Crypted = 0x01 };
		Q_DECLARE_FLAGS(ValueFlags, ValueFlag)
		enum OpenFlag { IncludeGlobals = 0x01, SimpleConfig = 0x00 };
		Q_DECLARE_FLAGS(OpenFlags, OpenFlag)
		// If file is empty, then profile settings are loaded
		explicit Config(const QString &file = QString(), OpenFlags flags = IncludeGlobals, const QString &backend = QString());
		explicit Config(const QStringList &files, OpenFlags flags = IncludeGlobals, const QString &backend = QString());
		Config(const Config &other);
		Config(const QExplicitlySharedDataPointer<ConfigPrivate> &other);
		virtual ~Config();
#ifdef DOC
		QStringList groupList() const;
		bool hasGroup(const QString &name) const;
		const ConfigGroup group(const QString &name) const;
		ConfigGroup group(const QString &name);
		void removeGroup(const QString &name);
#endif
		void sync();
	private:
		friend class ModuleManager;
		friend class ConfigGroup;
		friend class ConfigBase;
		QExplicitlySharedDataPointer<ConfigPrivate> p;
	};

	class LIBQUTIM_EXPORT ConfigGroup : public ConfigBase
	{
	public:
		ConfigGroup(const ConfigGroup &other);
		ConfigGroup(const QExplicitlySharedDataPointer<ConfigGroupPrivate> &other);
		virtual ~ConfigGroup();
		QString name() const;
		bool isValid() const;
		bool isMap() const;
		bool isArray() const;
		bool isValue() const;

		int arraySize() const;
		const ConfigGroup at(int index) const;
		ConfigGroup at(int index);
		void removeAt(int index);

		ConfigGroup parent();
		const ConfigGroup parent() const;

		Config config();
		const Config config() const;

		template<typename T>
		T value(const QString &key, const T &def, Config::ValueFlags type = Config::Normal) const;
		QVariant value(const QString &key, const QVariant &def, Config::ValueFlags type = Config::Normal) const;
		void setValue(const QString &key, const QVariant &value, Config::ValueFlags type = Config::Normal);

		void sync();
	private:
		ConfigGroup();
		friend class Config;
		friend class ConfigBase;
		QExplicitlySharedDataPointer<ConfigGroupPrivate> p;
	};

	inline const ConfigGroup ConfigBase::constGroup(const QString &group) const
	{
		return const_cast<const ConfigBase *>(this)->group(group);
	}

	template<typename T>
	Q_INLINE_TEMPLATE T ConfigGroup::value(const QString &key, const T &def, Config::ValueFlags type) const
	{
		return value(key, QVariant(def), type).value<T>();
	}
}

#endif // CONFIGBASE_H
