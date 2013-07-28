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

#include "metaobjectbuilder.h"
#include <QPair>
#include <QVector>
#include <QMap>
#include <QMetaClassInfo>

namespace qutim_sdk_0_3
{
class MetaObjectBuilderPrivate
{
public:
	QByteArray name;
	const QMetaObject *parent;
	QList<QPair<QByteArray, QByteArray> > classInfos;
};

MetaObjectBuilder::MetaObjectBuilder(const QByteArray &name, const QMetaObject *parent)
	: d_ptr(new MetaObjectBuilderPrivate)
{
	Q_D(MetaObjectBuilder);
	d->name = name;
	d->parent = parent;
}

MetaObjectBuilder::~MetaObjectBuilder()
{
}

void MetaObjectBuilder::addClassInfo(const QByteArray &name, const QByteArray &value)
{
	d_func()->classInfos << qMakePair(name, value);
}

static int ensureIndex(const QByteArray &name, QByteArray &data, QMap<QByteArray, int> &map)
{
	int index = map.value(name, -1);
	if (index == -1) {
		index = data.size();
		data += name;
		data += '\0';
		map.insert(name, index);
	}
	return index;
}

QMetaObject *MetaObjectBuilder::generate()
{
    return NULL;
//	Q_D(MetaObjectBuilder);
//	QMetaObject *meta = new QMetaObject;
//	meta->d.superdata = d->parent;
//	QByteArray stringdata;
//	QVector<uint> data(14, 0);
//	data[0] = 4; // revision
//	data[1] = 0; // classname
//	stringdata += d->name;
//	stringdata += '\0';
//	if (!d->classInfos.isEmpty()) {
//		data[2] = d->classInfos.count();
//		data[3] = data.size();
//		QMap<QByteArray, int> map;
//		for (int i = 0; i < d->classInfos.size(); i++) {
//			const QPair<QByteArray, QByteArray> &pair = d->classInfos.at(i);
//			data += ensureIndex(pair.second, stringdata, map);
//			data += ensureIndex(pair.first, stringdata, map);
//		}
//	}
//	data += 0; // eod
//	{
//		void *tmp1 = malloc(stringdata.size() + 1);
//		memcpy(tmp1, stringdata.constData(), stringdata.size() + 1);
//		meta->d.stringdata = reinterpret_cast<char*>(tmp1);
//		void *tmp2 = malloc(data.size() * sizeof(uint));
//		memcpy(tmp2, data.constData(), data.size() * sizeof(uint));
//		meta->d.data = reinterpret_cast<uint*>(tmp2);
//	}
//	meta->d.extradata = 0;
//	return meta;
}

const char *MetaObjectBuilder::info(const QMetaObject *meta, const char *name)
{
	int index = meta->indexOfClassInfo(name);
	return index == -1 ? 0 : meta->classInfo(index).value();
}
}

