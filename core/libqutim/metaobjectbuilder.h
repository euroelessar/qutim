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

	void addClassInfo(const QByteArray &name, const QByteArray &value);
	QMetaObject *generate();

	static const char *info(const QMetaObject *meta, const char *name);
private:
	QScopedPointer<MetaObjectBuilderPrivate> d_ptr;
};
}

#endif // METAOBJECTBUILDER_H

