#include "plugpackage.h"
#include "utils/plugversion.h"
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
	if (properties.value("name").isEmpty())
 		return false;
	if (properties.value("type").isEmpty())
 		return false;
	if (!plugVersion(properties.value("version")).isValid())
 		return false;
//	if (properties["url"].section(".",-1)!="zip")
// 		return false;
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

