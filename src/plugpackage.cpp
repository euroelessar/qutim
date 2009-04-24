#include "plugpackage.h"

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
	return QString();
}

bool packageInfo::isValid() {
	return true;
}


ItemData::ItemData(itemType type,  QIcon icon ,packageInfo packageItem, quint32 id) {
	this->id = id;
	this->type = type;
	this->icon = icon;
	this->packageItem = packageItem;
}

ItemData::ItemData() {

}

