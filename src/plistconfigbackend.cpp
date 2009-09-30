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

#include "libqutim/jsonfile.h"
#include "modulemanagerimpl.h"
#include <QDebug>
#include "plistconfigbackend.h"
#include <QDomDocument>
#include <QFile>

namespace Core
{
static CoreModuleHelper<PListConfigBackend> z_plist_config_static(
    QT_TRANSLATE_NOOP("Plugin", "PList config"),
    QT_TRANSLATE_NOOP("Plugin", "Additional qutIM config realization for Apple plists")
);

void PListConfigBackend::generate(const QString& file, const qutim_sdk_0_3::ConfigEntry::Ptr& entry)
{

}


ConfigEntry::Ptr PListConfigBackend::generateConfigEntry(const QDomNode& val)
{
    ConfigEntry::Ptr entry(new ConfigEntry);
    if (val.isNull()) {
		entry.data()->type = ConfigEntry::Invalid;
        return entry;
	}
    QString key = "";
	entry->type = ConfigEntry::Map;
    for (QDomNode node = val.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        QDomElement element = node.toElement();
        if (element.nodeName()=="key")
            key = element.text();
        else
        {
			ConfigEntry::Ptr value(new ConfigEntry);
            if (element.nodeName()=="true") {
                value->value = QVariant(true);
				value->type = ConfigEntry::Value;
			}
            else if (element.nodeName()=="false") {
                value->value = QVariant(false);
				value->type = ConfigEntry::Value;
			}
            else if (element.nodeName()=="real") {
                value->value = QVariant(element.text().toDouble());
				value->type = ConfigEntry::Value;
			}
            else if (element.nodeName()=="string") {
                value->value = QVariant(element.text());
				value->type = ConfigEntry::Value;
			}
            else if (element.nodeName()=="integer") {
                value->value = QVariant(element.text().toInt());
				value->type = ConfigEntry::Value;
			}
            else if (element.nodeName()=="dict") {
				value = generateConfigEntry(node);
			}
			entry->map.insert(key,value);
        }
    }
    return entry;
}


QVariant PListConfigBackend::genetateQVariant(const qutim_sdk_0_3::ConfigEntry::Ptr& entry)
{

}


ConfigEntry::Ptr PListConfigBackend::parse(const QString& file)
{
    QDomDocument plist;
    QFile input (file);
    bool isValid = input.open(QIODevice::ReadOnly) && plist.setContent(&input);
    input.close();
    QDomElement root = plist.documentElement();

    if (!isValid) {
		ConfigEntry::Ptr entry(new ConfigEntry);
		entry.data()->type = ConfigEntry::Invalid;
        return entry;
    }
    return generateConfigEntry(root.firstChild());
}

}
