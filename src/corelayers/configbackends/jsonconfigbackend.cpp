/*
	<one line to give the program's name and a brief idea of what it does.>
	Copyright (C) <2010>  <Sidorov Aleksey sauron@citadelspb.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "jsonconfigbackend.h"
#include "plistconfigbackend.h"
#include "libqutim/jsonfile.h"
#include "modulemanagerimpl.h"
#include <QDebug>

namespace Core
{
	static CoreModuleHelper2<JsonConfigBackend, PListConfigBackend> z_config_static(
			QT_TRANSLATE_NOOP("Plugin", "JSON config"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM config realization. Based on JSON."),
			QT_TRANSLATE_NOOP("Plugin", "PList config"),
			QT_TRANSLATE_NOOP("Plugin", "Additional qutIM config realization for Apple plists")
			);

	void JsonConfigBackend::generate(const QString& file, const ConfigEntry::Ptr& entry)
	{
		JsonFile jsonFile (file);

		if (entry->type == ConfigEntry::Invalid) {
			qWarning() << "Invalid entry";
		}

		if (entry->type == ConfigEntry::Value) {
		}

		QVariant map = generateQVariant(entry);
		if(map.isNull() || !map.isValid())
			map = QVariantMap();
//		qDebug() << map;
		jsonFile.save(map);
	}


	ConfigEntry::Ptr JsonConfigBackend::parse(const QString& file)
	{
		QVariant map;
		JsonFile jsonFile (file);
		jsonFile.load(map);
		if(map.type() != QVariant::Map)
			map = QVariantMap();
		return generateConfigEntry(map);
	}

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

	inline QVariant variantFromString(const QString &s)
	{
		if (s.startsWith(QLatin1Char('@'))) {
			if (s.endsWith(")")) {
				if (s.startsWith(QLatin1String("@ByteArray("))) {
					return QVariant(QByteArray::fromBase64(s.toLatin1().mid(11, s.size() - 12)));
//					return QVariant(s.toLatin1().mid(11, s.size() - 12));
				} else if (s.startsWith(QLatin1String("@Variant("))) {
					QByteArray a(s.toLatin1().mid(9));
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

	ConfigEntry::Ptr JsonConfigBackend::generateConfigEntry (const QVariant &val)
	{
		ConfigEntry::Ptr entry(new ConfigEntry);
		entry->type = ConfigEntry::Invalid;
		switch (val.type()) {
			case QVariant::Invalid:
				entry->type = ConfigEntry::Invalid;
				return entry;
			case QVariant::Map: {
				QVariantMap m(val.toMap());
				QVariantMap::const_iterator i;
				entry->type = ConfigEntry::Map;
				for (i = m.constBegin(); i != m.constEnd(); ++i) {
					entry->map.insert(i.key(),generateConfigEntry(i.value()));
				}
				break;
			}
			case QVariant::StringList:
			case QVariant::List: {
				QVariantList m(val.toList());
				entry->type = ConfigEntry::Array;
				foreach (const QVariant &v, m) {
					entry->array.append(generateConfigEntry(v));
				}
				break;
			}
		}
		entry->type |= ConfigEntry::Value;
		if(val.type() == QVariant::String)
			entry->value = variantFromString(val.toString());
		else
			entry->value = val;
		return entry;
	}

	QVariant variantToString(const QVariant &v)
	{
		QString result;

		switch (v.type()) {
			case QVariant::StringList:
			case QVariant::List:
			case QVariant::Map:
				return v;

			case QVariant::Invalid:
				result = QLatin1String("@Invalid()");
				break;

			case QVariant::ByteArray: {
				QByteArray a = v.toByteArray();
				result = QLatin1String("@ByteArray(");
				result += a.toBase64();
//				result += QString::fromLatin1(a.constData(), a.size());
				result += QLatin1Char(')');
				break;
			}

			case QVariant::String:
			case QVariant::LongLong:
			case QVariant::ULongLong:
			case QVariant::Int:
			case QVariant::UInt:
			case QVariant::Bool:
			case QVariant::Double:
			case QVariant::KeySequence: {
				result = v.toString();
				if (result.startsWith(QLatin1Char('@')))
					result.prepend(QLatin1Char('@'));
				break;
			}
			case QVariant::Rect: {
				QRect r = qvariant_cast<QRect>(v);
				result += QLatin1String("@Rect(");
				result += QString::number(r.x());
				result += QLatin1Char(' ');
				result += QString::number(r.y());
				result += QLatin1Char(' ');
				result += QString::number(r.width());
				result += QLatin1Char(' ');
				result += QString::number(r.height());
				result += QLatin1Char(')');
				break;
			}
			case QVariant::Size: {
				QSize s = qvariant_cast<QSize>(v);
				result += QLatin1String("@Size(");
				result += QString::number(s.width());
				result += QLatin1Char(' ');
				result += QString::number(s.height());
				result += QLatin1Char(')');
				break;
			}
			case QVariant::Point: {
				QPoint p = qvariant_cast<QPoint>(v);
				result += QLatin1String("@Point(");
				result += QString::number(p.x());
				result += QLatin1Char(' ');
				result += QString::number(p.y());
				result += QLatin1Char(')');
				break;
			}

			default: {
				QByteArray a;
				{
					QDataStream s(&a, QIODevice::WriteOnly);
					s.setVersion(QDataStream::Qt_4_0);
					s << v;
				}

				result = QLatin1String("@Variant(");
				result += QString::fromLatin1(a.constData(), a.size());
				result += QLatin1Char(')');
				break;
			}
		}

		return QVariant(result);
	}

	QVariant JsonConfigBackend::generateQVariant(const ConfigEntry::Ptr& entry)
	{
		QVariant val;
		if(entry->type & ConfigEntry::Map)
		{
			QVariantMap m; //TODO need optimization!!
			ConfigEntry::EntryMap::const_iterator i;
			for (i = entry->map.constBegin(); i != entry->map.constEnd(); ++i) {
				QVariant val = generateQVariant(i.value());
				if(val.isValid() && !val.isNull())
					m.insert(i.key(),val);
			}
			val = m.isEmpty() ? QVariant() : QVariant(m);
		}
		else if(entry->type & ConfigEntry::Array)
		{
			QVariantList l;
			foreach (const ConfigEntry::Ptr& e, entry->array) {
				QVariant val = generateQVariant(e);
				if(val.isValid() && !val.isNull())
					l.append(val);
			}
			val = l.isEmpty() ? QVariant() : QVariant(l);
		}
		else if(entry->type & ConfigEntry::Value)
		{
			val = variantToString(entry->value);
		}
		return val;
	}
}
