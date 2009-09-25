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
	class LIBQUTIM_EXPORT ObjectGenerator
	{
		Q_DISABLE_COPY(ObjectGenerator)
	public:
		inline QObject *generate() const
		{ return generateHelper(); }
		template<typename T>
		inline T *generate() const
		{ return extends<T>() ? qobject_cast<T *>(generateHelper()) : 0; }
		inline QObject *generate(const QMetaObject *super) const
		{ return extends(super) ? generateHelper() : 0; }
		virtual const QMetaObject *metaObject() const = 0;
		inline bool extends(const QMetaObject *super) const
		{
			const QMetaObject *meta = metaObject();
			while(meta && ((meta = meta->superClass()) != super));
			return meta == super;
		}
		template<typename T>
		inline bool extends() const
		{
			const QMetaObject *meta = metaObject();
			while(meta && ((meta = meta->superClass()) != &T::staticMetaObject));
			return meta == &T::staticMetaObject;
		}
	protected:
		virtual QObject *generateHelper() const = 0;
		inline ObjectGenerator() {}
		mutable QPointer<QObject> m_object;
	};

	template<typename T>
	class LIBQUTIM_EXPORT GeneralGenerator : public ObjectGenerator
	{
		Q_DISABLE_COPY(GeneralGenerator)
	public:
		inline GeneralGenerator() {}
	protected:
		virtual QObject *generateHelper() const
		{
			if(m_object.isNull())
				m_object = new T();
			return m_object.data();
		}
		virtual const QMetaObject *metaObject() const
		{
			return &T::staticMetaObject;
		}
	};
}

#endif // OBJECTGENERATOR_H
