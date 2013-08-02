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

#include "jsonconfigbackend.h"
#include <qutim/jsonfile.h>
#include <qutim/json.h>
#include <QFile>
#include <qutim/debug.h>
#include <QRect>
#include <QStringList>

namespace Core
{
	inline QStringList splitArgs(const QString &s, int idx)
	{
		int l = s.length();
		Q_ASSERT(l > 0);
		Q_ASSERT(s.at(idx) == QLatin1Char('('));
		Q_ASSERT(s.at(l - 1) == QLatin1Char(')'));

		QStringList result;
		QString item;

		for (++idx; idx < l; ++idx) {
			QChar c = s.at(idx);
			if (c == QLatin1Char(')')) {
				Q_ASSERT(idx == l - 1);
				result.append(item);
			} else if (c == QLatin1Char(' ')) {
				result.append(item);
				item.clear();
			} else {
				item.append(c);
			}
		}

		return result;
	}

	QVariant variantFromString(const QString &s)
	{
		if (s.startsWith(QLatin1Char('@'))) {
			if (s.endsWith(")")) {
				if (s.startsWith(QLatin1String("@ByteArray("))) {
					return QVariant(QByteArray::fromBase64(s.toLatin1().mid(11, s.size() - 12)));
//					return QVariant(s.toLatin1().mid(11, s.size() - 12));
				} else if (s.startsWith(QLatin1String("@Variant("))) {
					QByteArray a(QByteArray::fromBase64(s.toLatin1().mid(9, s.size() - 10)));
					QDataStream stream(&a, QIODevice::ReadOnly);
					stream.setVersion(QDataStream::Qt_4_5);
					QVariant result;
					stream >> result;
					return result;
				} else if (s.startsWith(QLatin1String("@Rect("))) {
					QStringList args = splitArgs(s, 5);
					if (args.size() == 4)
						return QVariant(QRect(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt()));
				} else if (s.startsWith(QLatin1String("@Size("))) {
					QStringList args = splitArgs(s, 5);
					if (args.size() == 2)
						return QVariant(QSize(args[0].toInt(), args[1].toInt()));
				} else if (s.startsWith(QLatin1String("@Point("))) {
					QStringList args = splitArgs(s, 6);
					if (args.size() == 2)
						return QVariant(QPoint(args[0].toInt(), args[1].toInt()));
				} else if (s == QLatin1String("@Invalid()")) {
					return QVariant();
				}

			}
			if (s.startsWith(QLatin1String("@@")))
				return QVariant(s.mid(1));
		}
		return QVariant(s);
	}
		
	bool variantGeneratorExt(QString &err, QByteArray &result, const QVariant &val, int indent)
	{
		Q_UNUSED(err);
		Q_UNUSED(indent);
		result += '\"';
		switch (val.type()) {
			case QVariant::ByteArray: {
				QByteArray a = val.toByteArray();
				result += "@ByteArray(";
				result += a.toBase64();
//				result += QString::fromLatin1(a.constData(), a.size());
				result += ')';
				break;
			}
			case QVariant::KeySequence: {
				QString tmp = val.toString();
				if (tmp.startsWith(QLatin1Char('@')))
					tmp.prepend(QLatin1Char('@'));
				result += tmp.toUtf8();
				break;
			}
			case QVariant::Rect: {
				QRect r = qvariant_cast<QRect>(val);
				result += "@Rect(";
				result += QByteArray::number(r.x());
				result += ' ';
				result += QByteArray::number(r.y());
				result += ' ';
				result += QByteArray::number(r.width());
				result += ' ';
				result += QByteArray::number(r.height());
				result += ')';
				break;
			}
			case QVariant::Size: {
				QSize s = qvariant_cast<QSize>(val);
				result += "@Size(";
				result += QByteArray::number(s.width());
				result += ' ';
				result += QByteArray::number(s.height());
				result += ')';
				break;
			}
			case QVariant::Point: {
				QPoint p = qvariant_cast<QPoint>(val);
				result += "@Point(";
				result += QByteArray::number(p.x());
				result += ' ';
				result += QByteArray::number(p.y());
				result += ')';
				break;
			}

			default: {
				QByteArray a;
				{
					QDataStream s(&a, QIODevice::WriteOnly);
					s.setVersion(QDataStream::Qt_4_5);
					s << val;
				}

				result += "@Variant(";
				result += a.toBase64();
				result += ')';
				break;
			}
		}
		result += '\"';
		return true;
	}
	
	void validateVariant(QVariant *var) 
	{
		switch (var->type()) {
		case QVariant::String: {
			var->setValue(variantFromString(var->toString()));
			break;
		} 
		case QVariant::Map: {
			QVariantMap *map = reinterpret_cast<QVariantMap*>(var->data());
			QVariantMap::iterator it = map->begin();
			for (; it != map->end(); it++) {
				QVariant &value = it.value();
				if (value.type() == QVariant::Map || value.type() == QVariant::List 
					|| value.type() == QVariant::String) {
					validateVariant(&value);
				}
			}
			break;
		}
		case QVariant::List: {
			QVariantList *list = reinterpret_cast<QVariantList*>(var->data());
			for (int i = 0; i < list->size(); i++) {
				QVariant &value = (*list)[i];
				if (value.type() == QVariant::Map || value.type() == QVariant::List 
					|| value.type() == QVariant::String) {
					validateVariant(&value);
				}
			}
			break;
		}
		default:
			break;
		}
	}

	QVariant JsonConfigBackend::load(const QString &fileName)
	{
		JsonFile file(fileName);
		QVariant var;
		file.load(var);
		if (var.type() == QVariant::Map || var.type() == QVariant::List 
			|| var.type() == QVariant::String) {
			validateVariant(&var);
		}
		return var;
	}

	void JsonConfigBackend::save(const QString &fileName, const QVariant &entry)
	{
		QFile file(fileName);
		if (file.open(QFile::WriteOnly | QIODevice::Text)) {
			QByteArray data;
			Json::generate(data, entry, 2, variantGeneratorExt);
//			qDebug() << QString::fromUtf8(data, data.size());
			file.write(data);
			file.close();
		}
	}
}

