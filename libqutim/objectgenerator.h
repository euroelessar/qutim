/****************************************************************************
 *  objectgenerator.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef OBJECTGENERATOR_H
#define OBJECTGENERATOR_H

#include "libqutim_global.h"
#include <QPointer>

namespace qutim_sdk_0_3
{
	struct ObjectGeneratorPrivate;

	class LIBQUTIM_EXPORT ObjectGenerator
	{
		Q_DECLARE_PRIVATE(ObjectGenerator)
		Q_DISABLE_COPY(ObjectGenerator)
	protected:
		ObjectGenerator();
		ObjectGenerator(ObjectGeneratorPrivate &priv);
	public:
		typedef ObjectGeneratorPrivate Data;
		virtual ~ObjectGenerator();

		ObjectGenerator *addProperty(const QByteArray &name, const QVariant &value);

		inline QObject *generate() const
		{ return generateHelper2(); }
		template<typename T>
		inline T *generate() const
		{ return extends<T>() ? qobject_cast<T *>(generateHelper2()) : 0; }
		inline QObject *generate(const QMetaObject *super) const
		{ return extends(super) ? generateHelper2() : 0; }
		inline QObject *generate(const char *id) const
		{ return extends(id) ? generateHelper2() : 0; }

		virtual const QMetaObject *metaObject() const = 0;
		virtual bool hasInterface(const char *id) const = 0;

		// TODO: There should be a way for getting interfaces list
//		virtual QList<const char *> interfaces() const = 0;
		bool extends(const QMetaObject *super) const;
		inline bool extends(const char *id) const
		{ return id && hasInterface(id); }
		template<typename T>
		inline bool extends() const
		{ return extends_helper<T>(reinterpret_cast<T *>(0)); }
	protected:
		template<typename T>
		inline bool extends_helper(const QObject *) const
		{ return extends(&T::staticMetaObject); }
		template<typename T>
		inline bool extends_helper(const void *) const
		{ return extends(qobject_interface_iid<T *>()); }

		QObject *generateHelper2() const;
		virtual QObject *generateHelper() const = 0;
		QScopedPointer<ObjectGeneratorPrivate> d_ptr;
	public:
		inline Data *data() { return d_ptr.data(); }
	};

	template<typename T, typename I0 = void,
	typename I1 = void, typename I2 = void, typename I3 = void,
	typename I4 = void, typename I5 = void, typename I6 = void,
	typename I7 = void, typename I8 = void, typename I9 = void>
	class GeneralGenerator : public ObjectGenerator
	{
		Q_DISABLE_COPY(GeneralGenerator)
	public:
		inline GeneralGenerator() {}
	protected:
		virtual QObject *generateHelper() const
		{
			return new T();
		}
		virtual const QMetaObject *metaObject() const
		{
			return &T::staticMetaObject;
		}
		virtual bool hasInterface(const char *id) const
		{
			return !qstrcmp(qobject_interface_iid<I0 *>(), id)
					|| !qstrcmp(qobject_interface_iid<I1 *>(), id)
					|| !qstrcmp(qobject_interface_iid<I2 *>(), id)
					|| !qstrcmp(qobject_interface_iid<I3 *>(), id)
					|| !qstrcmp(qobject_interface_iid<I4 *>(), id)
					|| !qstrcmp(qobject_interface_iid<I5 *>(), id)
					|| !qstrcmp(qobject_interface_iid<I6 *>(), id)
					|| !qstrcmp(qobject_interface_iid<I7 *>(), id)
					|| !qstrcmp(qobject_interface_iid<I8 *>(), id)
					|| !qstrcmp(qobject_interface_iid<I9 *>(), id);
		}
	};


	template<typename T, typename I0 = void,
	typename I1 = void, typename I2 = void, typename I3 = void,
	typename I4 = void, typename I5 = void, typename I6 = void,
	typename I7 = void, typename I8 = void, typename I9 = void>
	class SingletonGenerator : public GeneralGenerator<T, I0, I1, I2, I3, I4, I5, I6, I7, I8, I9>
	{
		Q_DISABLE_COPY(SingletonGenerator)
	public:
		inline SingletonGenerator() {}
	protected:
		virtual QObject *generateHelper() const
		{
			if(m_object.isNull())
				m_object = new T;
			return m_object.data();
		}
		mutable QPointer<QObject> m_object;
	};
}

#endif // OBJECTGENERATOR_H
