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
#include "plugpackage.h"
#include "plugxmlhandler.h"
#include <QTextCodec>
#include <QTextStream>
// #include <QDebug>
packageInfo::packageInfo() {

}


packageInfo::packageInfo(QString name, QStringList packageFiles, QString type, QString description, QString author, QString licence,quint16 id) {
	properties["name"] = name;
	files = packageFiles;
	properties["type"] = type;
	properties["description"] = description;
	properties["author"] = author;
	this->id = id;
}

QString packageInfo::toString() {
	QDomElement element = plugXMLHandler::createElementFromPackage(*this);

	static QTextCodec *utf = QTextCodec::codecForName( "utf-8" );
	QString xml;
	{
		QTextStream stream(&xml, QIODevice::WriteOnly);
		stream.setCodec( utf );
		element.save( stream, 0, QDomNode::EncodingFromTextStream );
	}
	return xml;
}

bool packageInfo::isValid() {
	if (properties.value("name").isEmpty()) {
		this->ErrorString = QObject::tr("Package name is empty");
 		return false;
	}
	if (properties.value("type").isEmpty()) {
		this->ErrorString = QObject::tr("Package type is empty");
		return false;
	}
	if (!plugVersion(properties.value("version")).isValid()) {
		this->ErrorString = QObject::tr("Invalid package version");
 		return false;
	}
	const QString &platform = properties.value("platform");
	if (platform.isEmpty()||platform=="all")
		return true;
	if (platform != QLatin1String(QT_BUILD_KEY)) {
		this->ErrorString = QObject::tr("Wrong platform");
		return false;
	}
	return true;
}


ItemData::ItemData(itemType type,
                   QIcon icon,
                   packageInfo packageItem,
                   packageAttribute attribute,
                   packageChecked checked)
{
	this->type = type;
	this->icon = icon;
	this->packageItem = packageItem;
	this->attribute = attribute;
	this->checked = checked;
	this->name = packageItem.properties.value("type")+"/"+packageItem.properties.value("name");
}

ItemData::ItemData() {
}

