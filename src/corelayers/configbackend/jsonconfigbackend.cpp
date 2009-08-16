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
#include <libqutim/jsonfile.h>
#include <QDebug>

void Core::JsonConfigBackend::generate(const QString& file, const qutim_sdk_0_3::ConfigEntry::Ptr& entry)
{
    JsonFile jsonFile (file);

    if (entry.data()->type == ConfigEntry::Invalid) {
        qWarning() << "Invalid entry";
    }

    if (entry.data()->type == ConfigEntry::Value) {
    }

    QVariant map;
    jsonFile.save(map);
}


Core::JsonConfigBackend::JsonConfigBackend()
{

}


qutim_sdk_0_3::ConfigEntry::Ptr Core::JsonConfigBackend::parse(const QString& file)
{
    QVariant map;
    JsonFile jsonFile (file);
    jsonFile.load(map);
    return generateConfigEntry(map);
}

qutim_sdk_0_3::ConfigEntry::Ptr Core::JsonConfigBackend::generateConfigEntry (const QVariant &val)
{
    qutim_sdk_0_3::ConfigEntry::Ptr entry;
    switch (val.type()) {
        case QVariant::Invalid:
            entry.data()->type = ConfigEntry::Invalid;
            break;
        case QVariant::Map: {
            QVariantMap m(val.toMap());
            QVariantMap::const_iterator i;
            entry.data()->type = ConfigEntry::Map;
            for (i = m.constBegin(); i != m.constEnd(); ++i) {
                entry.data()->map.insert(i.key(),generateConfigEntry(i.value()));
            }
            break;
        }
        case QVariant::List: {
             QVariantList m(val.toList());
             entry.data()->type = ConfigEntry::Array;
             foreach (const QVariant &v, m) {
                 entry.data()->array.append(generateConfigEntry(v));
                 }
             break;
        }
                //    case QVariant::StringList: //x3
                //        QStringList m(val.toStringList());
                //        entry.data()->type == ConfigEntry::Array;
                //        foreach (const QString &v, m) {
                //            entry.data()->array.append(generateConfigEntry(QVariant(v)));
                //        }
                //        break;
        default: {
                 entry.data()->value = val;
                 entry.data()->type = ConfigEntry::Value;
                 break;
        }
    }
    return entry;
}

QVariant Core::JsonConfigBackend::genetateQVariant(const qutim_sdk_0_3::ConfigEntry::Ptr& entry)
{
    QVariant val;

    switch (entry.data()->type) {
//        case ConfigEntry::Invalid: //moved to default
//                break;
//        case ConfigEntry::deleted:
//                break;
//        case ConfigEntry::dirty:
//                break;
        case ConfigEntry::Map: {
            QVariantMap m; //TODO need optimization!!
            ConfigEntry::EntryMap::const_iterator i;
            for (i = entry.data()->map.constBegin(); i != entry.data()->map.constEnd(); ++i) {
                m.insert(i.key(),genetateQVariant(i.value()));
            }
            val = QVariant(m);
            break;
        }
        case ConfigEntry::Array: {
            QVariantList l;
            foreach (const qutim_sdk_0_3::ConfigEntry::Ptr& e, entry.data()->array) {
                l.append(genetateQVariant(e));
            }
            val = QVariant(l);
            break;
        }
        case ConfigEntry::Value: {
            val = entry.data()->value;
            break;
        }
        default:
            break;
    }
    return val;
}
