/****************************************************************************
 *  configbase.h
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

#ifndef CONFIGBASE_H
#define CONFIGBASE_H

#include "libqutim_global.h"
#include <QVariant>
#include <QSharedPointer>

namespace qutim_sdk_0_3
{
#ifndef Q_QDOC
	namespace EnumDetectorHelper
	{
		typedef quint8 Yes;
		typedef quint16 No;
		
		// Check if type can be casted to int
		template <typename T> Yes is_int_type(int) { return Yes(); }
		template <typename T> No is_int_type(...) { return No(); }
		
		template <int Defined, int Size>
		class Helper
		{
		public:
			static No value() { return No(); }
		};
		
		template <>
		class Helper<0, sizeof(Yes)>
		{
		public:
			static Yes value() { return Yes(); }
		};
		
		template <typename T, int IsEnum>
		class VariantCastHelper
		{
		public:
			static QVariant convertToVariant(const T &t)
			{ return qVariantFromValue(t); }
			static T convertFromVariant(const QVariant &t)
			{ return qVariantValue<T>(t); }
		};
		
		template <typename T>
		class VariantCastHelper <T, sizeof(Yes)>
		{
		public:
			static QVariant convertToVariant(const T &t)
			{ return qVariantFromValue(int(t)); }
			static T convertFromVariant(const QVariant &v)
			{ return static_cast<T>(v.toInt()); }
		};
		
		template <typename T, int Defined>
		class VariantCastHelper3
		{
		public:
			static QVariant convertToVariant(const T &t)
			{
				return VariantCastHelper<T, sizeof(No)>::convertToVariant(t);
			}
			static T convertFromVariant(const QVariant &v)
			{
				return VariantCastHelper<T, sizeof(No)>::convertFromVariant(v);
			}
		};
		
		// Enums are not registered in Qt meta system and they can be casted to int easily
		template <typename T>
		class VariantCastHelper3 <T, 0>
		{
		public:
			static QVariant convertToVariant(const T &t)
			{
				return VariantCastHelper<T, sizeof(Helper<QMetaTypeId2<T>::Defined, sizeof(is_int_type<T>(*reinterpret_cast<T*>(0)))>::value())>::convertToVariant(t);
			}
			static T convertFromVariant(const QVariant &v)
			{
				return VariantCastHelper<T, sizeof(Helper<QMetaTypeId2<T>::Defined, sizeof(is_int_type<T>(*reinterpret_cast<T*>(0)))>::value())>::convertFromVariant(v);
			}
		};
		
		// Enums are not registered in Qt meta system, so check it before possibility of cast to int
		// because QByteArray has "operator int()" in private section
		template <typename T>
		class VariantCastHelper2
		{
		public:
			static QVariant convertToVariant(const T &t)
			{
				return VariantCastHelper3<T, QMetaTypeId2<T>::Defined>::convertToVariant(t);
			}
			static T convertFromVariant(const QVariant &v)
			{
				return VariantCastHelper3<T, QMetaTypeId2<T>::Defined>::convertFromVariant(v);
			}
		};
	}
	
	class ConfigGroup;
	class ConfigBasePrivate;
	class ConfigGroupPrivate;
	class ConfigPrivate;

	class LIBQUTIM_EXPORT ConfigBase
	{
	public:
		enum ValueFlag { Normal = 0x00, Crypted = 0x01 };
		Q_DECLARE_FLAGS(ValueFlags, ValueFlag)

		bool isValid() const;

		QStringList groupList() const;
		bool hasGroup(const QString &group) const;
		ConfigGroup group(const QString &group);
		const ConfigGroup group(const QString &group) const;
		inline  const ConfigGroup constGroup(const QString &group) const;
		void removeGroup(const QString &name);

		template<typename T>
		T value(const QString &key, const T &def = T(), ValueFlags type = Normal) const;
		QVariant value(const QString &key, const QVariant &def = QVariant(), ValueFlags type = Normal) const;
		template<typename T>
		void setValue(const QString &key, const T &value, ValueFlags type = Normal);
		void setValue(const QString &key, const QVariant &value, ValueFlags type = Normal);

		virtual void sync() = 0;
	protected:
		template <typename T>
		QVariant convertToVariant(const T &t) const
		{ return EnumDetectorHelper::VariantCastHelper2<T>::convertToVariant(t); }
		
		template <typename T>
		T convertFromVariant(const QVariant &v) const
		{ return EnumDetectorHelper::VariantCastHelper2<T>::convertFromVariant(v); }
		
		ConfigBase();
		virtual ~ConfigBase();
		bool isGroup() const;
		virtual void virtual_hook(int, void*) {}
	private:
		QExplicitlySharedDataPointer<ConfigBasePrivate> get_p() const;
	};
#endif

	class LIBQUTIM_EXPORT Config
#ifndef Q_QDOC
	 : public ConfigBase
#endif
	{
	public:
#ifdef Q_QDOC
		enum ValueFlag
		{
			Normal = 0x00,
			Crypted = 0x01
		};
#else // Q_QDOC
		typedef ConfigBase::ValueFlag ValueFlag;
		typedef ConfigBase::ValueFlags ValueFlags;
#endif // Q_QDOC
		enum OpenFlag
		{
			IncludeGlobals = 0x01,
			SimpleConfig = 0x00
		};
		Q_DECLARE_FLAGS(OpenFlags, OpenFlag)
		/*!
		  Contructs Config whith \a file as source. If file is empty
		  \"profile\" will be opened.

		  It is possible to choose certain \a backend, otherwise it
		  will be guessed by file extension, or if it is not possible
		  or no backend supports it default one will be used.
		*/
		explicit Config(const QString &file = QString(), OpenFlags flags = IncludeGlobals, const QString &backend = QString());
		/*!
		  Contructs Config with \a files as fallbacks.
		*/
		explicit Config(const QStringList &files, OpenFlags flags = IncludeGlobals, const QString &backend = QString());
		/*!
		  Contructs copy of \a other.
		*/
		Config(const Config &other);
#ifndef Q_QDOC
		Config(const QExplicitlySharedDataPointer<ConfigPrivate> &other);
#endif
		/*!
		  Assigns the value of the config \a other to this config.
		*/
		Config &operator =(const Config &other);
		/*!
		  Destructor.
		*/
		virtual ~Config();
#ifdef Q_QDOC
		/*!
		  Returns \b true if config is valid, otherwise return \b false.
		*/
		bool isValid() const;
		/*!
		  Returns list of children groups.
		*/
		QStringList groupList() const;
		/*!
		  Returns if \a name is in list of children groups.
		*/
		bool hasGroup(const QString &name) const;
		/*!
		  Returns group with \a name.
		*/
		ConfigGroup group(const QString &name);
		/*!
		  Returns group with \a name.
		  Convience const version.
		*/
		const ConfigGroup group(const QString &name) const;
		/*! \copydoc Config::group(const QString&) const */
		const ConfigGroup constGroup(const QString &name) const;
		/*!
		  Remove group with \a name.
		*/
		void removeGroup(const QString &name);
		/*!
		  \copydoc Config::value(const QString &,const QVariant &,ValueFlags) const
		  If value can not be casted to type \b T default T value
		  will be returned.
		*/
		template<typename T>
		T value(const QString &key, const T &def, ValueFlags type = Normal) const;
		/*!
		  Returns the value for setting \a key. If the setting doesn't
		  exist, returns \a def.
		*/
		QVariant value(const QString &key, const QVariant &def, ValueFlags type = Normal) const;
		/*!
		  Set value for setings \a key to \a value.
		*/
		void setValue(const QString &key, const QVariant &value, ValueFlags type = Normal);
#endif
		/*!
		  Flush all changes in config to drive
		*/
		void sync();
#ifndef Q_QDOC
	private:
		friend class ModuleManager;
		friend class ConfigGroup;
		friend class ConfigBase;
		QExplicitlySharedDataPointer<ConfigPrivate> p;
#endif
	};

	class LIBQUTIM_EXPORT ConfigGroup
#ifndef Q_QDOC
	: public ConfigBase
#endif
	{
	public:
		/*!
		  Constructs copy of \a other
		*/
		ConfigGroup(const ConfigGroup &other);
#ifndef Q_QDOC
		ConfigGroup(const QExplicitlySharedDataPointer<ConfigGroupPrivate> &other);
#endif
		/*!
		  Assignes the value of config group \a other to this one.
		*/
		ConfigGroup &operator =(const ConfigGroup &other);
		/*!
		  Destructor
		*/
		virtual ~ConfigGroup();
		/*!
		  Returns name of group.
		*/
		QString name() const;
		/*!
		  Returns \b true if group is map, otherwise \b false.

		  \note Config group may be also a value at the same time as map.
		*/
		bool isMap() const;
		/*!
		  Returns \b true if group is array, otherwise \b false.

		  \note Config group may be also a value at the same time as array.
		*/
		bool isArray() const;
		/*!
		  Returns \b true if group is simple value, otherwise \b false.

		  \note Config group may be also a map or array at the same time as value.
		*/
		bool isValue() const;
		/*!
		  Returns array size
		*/
		int arraySize() const;
		/*!
		  Returns group at \a index.
		  If there is no group at \a index invalid group will be returned.
		*/
		const ConfigGroup at(int index) const;
		/*!
		  Returns group at \a index.
		  If there is no group at \a index it will be created. If group is
		  map all map values will be erased.
		*/
		ConfigGroup at(int index);
		/*!
		  Remove group at \a index.
		*/
		void removeAt(int index);
		/*!
		  Return parent config group.
		*/
		ConfigGroup parent();
		/*!
		  Return parent config group.
		  Const convience method.
		*/
		const ConfigGroup parent() const;
		/*!
		  Return config of group.
		*/
		Config config();
		/*!
		  Return config of group.
		  Const convience method.
		*/
		const Config config() const;

#ifdef Q_QDOC
		/*! \copydoc Config::isValid() */
		bool isValid() const;
		/*! \copydoc Config::groupList() */
		QStringList groupList() const;
		/*! \copydoc Config::hasGroup() */
		bool hasGroup(const QString &name) const;
		/*! \copydoc Config::group(const QString&) */
		ConfigGroup group(const QString &name);
		/*! \copydoc Config::group(const QString&) const */
		const ConfigGroup group(const QString &name) const;
		/*! \copydoc Config::constGroup() */
		const ConfigGroup constGroup(const QString &name) const;
		/*! \copydoc Config::removeGroup() */
		void removeGroup(const QString &name);
		/*! \copydoc Config::value(const QString&,const T&,ValueFlags) const */
		template<typename T>
		T value(const QString &key, const T &def, ValueFlags type = Normal) const;
		/*! \copydoc Config::value(const QString&,const QVariant&,ValueFlags) const */
		QVariant value(const QString &key, const QVariant &def, ValueFlags type = Normal) const;
		/*! \copydoc Config::setValue() */
		void setValue(const QString &key, const QVariant &value, ValueFlags type = Normal);
#endif
		/*! \copydoc Config::sync() */
		void sync();
#ifndef Q_QDOC
	private:
		ConfigGroup();
		friend class Config;
		friend class ConfigBase;
		QExplicitlySharedDataPointer<ConfigGroupPrivate> p;
#endif
	};

#ifndef Q_QDOC
	inline const ConfigGroup ConfigBase::constGroup(const QString &group) const
	{
		return const_cast<const ConfigBase *>(this)->group(group);
	}

	template<typename T>
	Q_INLINE_TEMPLATE T ConfigBase::value(const QString &key, const T &def, Config::ValueFlags type) const
	{
		return convertFromVariant<T>(value(key, convertToVariant(def), type));
	}
	
	template<typename T>
	Q_INLINE_TEMPLATE void ConfigBase::setValue(const QString &key, const T &value, Config::ValueFlags type)
	{
		setValue(key, convertToVariant(value), type);
	}

#endif
}

#endif // CONFIGBASE_H
