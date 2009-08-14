#ifndef CONFIGBASE_H
#define CONFIGBASE_H

#include "libqutim_global.h"
#include <QVariant>
#include <QSharedPointer>

namespace qutim_sdk_0_3
{
	class ConfigGroup;
	class ConfigGroupPrivate;
	class ConfigPrivate;

	class LIBQUTIM_EXPORT ConfigBase
	{
	public:
		virtual QStringList groupList() const = 0;
		virtual bool hasGroup(const QString &group) const = 0;
		virtual ConfigGroup group(const QString &group) = 0;
		virtual const ConfigGroup group(const QString &group) const = 0;
		virtual void removeGroup(const QString &name) = 0;

		virtual void sync() = 0;
	protected:
		ConfigBase();
		virtual ~ConfigBase();
		virtual void virtual_hook(int, void*) {}
	};

	class LIBQUTIM_EXPORT Config : public ConfigBase
	{
	public:
		enum ValueFlag { Normal = 0x00, Crypted = 0x01 };
		Q_DECLARE_FLAGS(ValueFlags, ValueFlag)
		enum OpenFlag { IncludeGlobals = 0x01, SimpleConfig = 0x00 };
		Q_DECLARE_FLAGS(OpenFlags, OpenFlag)
		// If file is empty, then profile settings are loaded
		Config(const QString &file = QString(), OpenFlags flags = IncludeGlobals, const QString &backend = QString());

		QString name() const;

		QStringList groupList() const;
		bool hasGroup(const QString &name) const;
		const ConfigGroup group(const QString &name) const;
		ConfigGroup group(const QString &name);
		void removeGroup(const QString &name);

		void sync();
	private:
		friend class ConfigGroup;
		QExplicitlySharedDataPointer<ConfigPrivate> p;
	};

	class LIBQUTIM_EXPORT ConfigGroup : public ConfigBase
	{
	public:
		QString name() const;
		bool isValid() const;
		bool isMap() const;
		bool isArray() const;
		bool isValue() const;

		QStringList groupList() const;
		bool hasGroup(const QString &name) const;
		const ConfigGroup group(const QString &name) const;
		ConfigGroup group(const QString &name);
		void removeGroup(const QString &name);

		int arraySize() const;
		const ConfigGroup at(int index) const;
		ConfigGroup at(int index);
		void removeAt(int index);

		ConfigGroup parent();
		const ConfigGroup parent() const;

		Config config();
		const Config config() const;

		template<typename T>
		T readEntry(const QString &key, const T &def, Config::ValueFlag type = Config::Normal) const;
		QVariant readEntry(const QString &key, const QVariant &def, Config::ValueFlag type = Config::Normal) const;
		void writeEntry(const QString &key, const QVariant &value, Config::ValueFlag type = Config::Normal);

		void sync();
	private:
		ConfigGroup();
		friend class Config;
		QExplicitlySharedDataPointer<ConfigGroupPrivate> p;
	};

	template<typename T>
	Q_INLINE_TEMPLATE T ConfigGroup::readEntry(const QString &key, const T &def, Config::ValueFlag type) const
	{
		return readEntry(key, QVariant(def), type).value<T>();
	}
}

#endif // CONFIGBASE_H
