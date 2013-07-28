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

#include "json.h"

#ifndef K8JSON_INCLUDE_GENERATOR
# define K8JSON_INCLUDE_GENERATOR
# define K8JSON_INCLUDE_COMPLEX_GENERATOR
#endif

#include "../3rdparty/k8json/k8json.h"
//#include <k8json/k8json.h>
#include <QMetaProperty>

namespace qutim_sdk_0_3
{
	namespace Json
	{
		QString quote(const QString &str)
		{
			return K8JSON::quote(str);
		}

		bool isValidUtf8(const uchar *s, int maxLen, bool zeroInvalid)
		{
			return K8JSON::isValidUtf8(s, maxLen, zeroInvalid);
		}

		bool isValidUtf8(const char *s, int maxLen, bool zeroInvalid)
		{
			return isValidUtf8(reinterpret_cast<const uchar *>(s), maxLen, zeroInvalid);
		}

		bool isValidUtf8(const QByteArray &data, bool zeroInvalid)
		{
			return K8JSON::isValidUtf8(reinterpret_cast<const uchar *>(data.data()),
									   data.size(),
									   zeroInvalid);
		}

		const uchar *skipBlanks(const uchar *s, int *maxLength)
		{
			return K8JSON::skipBlanks(s, maxLength);
		}

		const char *skipBlanks(const char *s, int *maxLength)
		{
			return reinterpret_cast<const char *>(skipBlanks(reinterpret_cast<const uchar *>(s), maxLength));
		}

		const uchar *skipRecord(const uchar *s, int *maxLength)
		{
			return K8JSON::skipRec(s, maxLength);
		}

		const char *skipRecord(const char *s, int *maxLength)
		{
			return reinterpret_cast<const char *>(skipRecord(reinterpret_cast<const uchar *>(s),
															 maxLength));
		}

		const uchar *parseValue(QVariant &fvalue, const uchar *s, int *maxLength)
		{
			return K8JSON::parseValue(fvalue, s, maxLength);
		}

		const char *parseValue(QVariant &fvalue, const char *s, int *maxLength)
		{
			return reinterpret_cast<const char *>(parseValue(fvalue,
															 reinterpret_cast<const uchar *>(s),
															 maxLength));
		}

		const uchar *parseField(QString &fname, QVariant &fvalue, const uchar *s, int *maxLength)
		{
			return K8JSON::parseField(fname, fvalue, s, maxLength);
		}

		const char *parseField(QString &fname, QVariant &fvalue, const char *s, int *maxLength)
		{
			return reinterpret_cast<const char *>(parseField(fname,
															 fvalue,
															 reinterpret_cast<const uchar *>(s),
															 maxLength));
		}

		const uchar *parseRecord(QVariant &res, const uchar *s, int *maxLength)
		{
			return K8JSON::parseRecord(res, s, maxLength);
		}

		const  char *parseRecord(QVariant &res, const  char *s, int *maxLength)
		{
			return reinterpret_cast<const char *>(parseRecord(res,
															  reinterpret_cast<const uchar *>(s),
															  maxLength));
		}

		struct FunctionPointerHelper
		{
			generatorExt gc;
		};

		bool simpleMethod(void *udata, QString &err, QByteArray &res, const QVariant &val, int indent)
		{
			FunctionPointerHelper *helper = reinterpret_cast<FunctionPointerHelper*>(udata);
			return (*helper->gc)(err, res, val, indent);
		}

		bool generate(QByteArray &res, const QVariant &val, int indent, generatorExt cb, QString *err)
		{
			QScopedPointer<QString> errPtr;
			if (!err)
				errPtr.reset(err = new QString());
			K8JSON::generatorCB gcb = cb ? &simpleMethod : 0;
			FunctionPointerHelper helper = { cb };
			return K8JSON::generateExCB(cb ? &helper : 0, gcb, *err, res, val, indent);
		}

		bool generate(QByteArray &res, const QVariant &val, int indent, QString *err)
		{
			QScopedPointer<QString> errPtr;
			if (!err)
				errPtr.reset(err = new QString());
			return K8JSON::generateEx(*err, res, val, indent);
		}

		QVariant parse(const QByteArray &data)
		{
			QVariant res;
			int len = data.size();
			K8JSON::parseRecord(res, reinterpret_cast<const uchar *>(data.constData()), &len);
			return res;
		}

		QByteArray generate(const QVariant &data, int indent)
		{
			QByteArray res;
			K8JSON::generate(res, data, indent);
			return res;
		}

		void parseToProperties(const QByteArray &json, QObject *obj)
		{
			QVariantMap map = parse(json).toMap();
			const QMetaObject *meta = obj->metaObject();
			for (int i = 0, size = meta->propertyCount(); i < size; i++) {
				QMetaProperty prop = meta->property(i);
				QVariantMap::iterator it = map.find(prop.name());
				if (it != map.end()) {
					QVariant var = it.value();
					if (var.canConvert(prop.type())) {
						prop.write(obj, var);
					}
				}
			}
		}
	}
}

