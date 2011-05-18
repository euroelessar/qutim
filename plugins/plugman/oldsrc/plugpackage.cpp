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

