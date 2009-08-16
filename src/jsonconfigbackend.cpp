/*
	<one line to give the program's name and a brief idea of what it does.>
	Copyright (C) <2009>  <Sidorov Aleksey sauron@citadelspb.com>

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
#include "libqutim/jsonfile.h"
#include "modulemanagerimpl.h"
#include <QDebug>

namespace Core
{
	static CoreModuleHelper<JsonConfigBackend> z_json_config_static(
			QT_TRANSLATE_NOOP("Plugin", "JSON config"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM config realization. Based on JSON.")
			);

	void JsonConfigBackend::generate(const QString& file, const ConfigEntry::Ptr& entry)
	{
		JsonFile jsonFile (file);

		if (entry->type == ConfigEntry::Invalid) {
			qWarning() << "Invalid entry";
		}

		if (entry->type == ConfigEntry::Value) {
		}

		QVariant map;
		jsonFile.save(map);
	}


	ConfigEntry::Ptr JsonConfigBackend::parse(const QString& file)
	{
		QVariant map;
		JsonFile jsonFile (file);
		jsonFile.load(map);
		return generateConfigEntry(map);
	}

	ConfigEntry::Ptr JsonConfigBackend::generateConfigEntry (const QVariant &val)
	{
		ConfigEntry::Ptr entry(new ConfigEntry);
		switch (val.type()) {
			case QVariant::Invalid:
				entry->type = ConfigEntry::Invalid;
				break;
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
					//    case QVariant::StringList: //x3
					//        QStringList m(val.toStringList());
					//        entry->type == ConfigEntry::Array;
					//        foreach (const QString &v, m) {
					//            entry->array.append(generateConfigEntry(QVariant(v)));
					//        }
					//        break;
//			default: {
//					 entry->value = val;
//					 entry->type = ConfigEntry::Value;
//					 break;
//			}
		}
		entry->type |= ConfigEntry::Value;
		entry->value = val;
		return entry;
	}

	QVariant JsonConfigBackend::genetateQVariant(const ConfigEntry::Ptr& entry)
	{
		QVariant val;

		switch (entry->type) {
	//        case ConfigEntry::Invalid: //moved to default
	//                break;
	//        case ConfigEntry::deleted:
	//                break;
	//        case ConfigEntry::dirty:
	//                break;
			case ConfigEntry::Map: {
				QVariantMap m; //TODO need optimization!!
				ConfigEntry::EntryMap::const_iterator i;
				for (i = entry->map.constBegin(); i != entry->map.constEnd(); ++i) {
					m.insert(i.key(),genetateQVariant(i.value()));
				}
				val = QVariant(m);
				break;
			}
			case ConfigEntry::Array: {
				QVariantList l;
				foreach (const ConfigEntry::Ptr& e, entry->array) {
					l.append(genetateQVariant(e));
				}
				val = QVariant(l);
				break;
			}
			case ConfigEntry::Value: {
				val = entry->value;
				break;
			}
			default:
				break;
		}
		return val;
	}
}
