/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef METAOBJECTBUILDER_H
#define METAOBJECTBUILDER_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	class MetaObjectBuilderPrivate;
	
	class LIBQUTIM_EXPORT MetaObjectBuilder
	{
		Q_DISABLE_COPY(MetaObjectBuilder)
		Q_DECLARE_PRIVATE(MetaObjectBuilder)
	public:
		MetaObjectBuilder(const QByteArray &name, const QMetaObject *parent  = 0);
		~MetaObjectBuilder();

		void addClassInfo(const QByteArray &name, class QByteArray &value);
		QMetaObject *generate();

		static const char *info(const QMetaObject *meta, const char *name);
	private:
		QScopedPointer<MetaObjectBuilderPrivate> d_ptr;
	};
}

#endif // METAOBJECTBUILDER_H
