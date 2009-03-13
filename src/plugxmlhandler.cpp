/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License version 2 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/

#include "plugxmlhandler.h"
#include <QDebug>



plugXMLHandler::packageInfo::packageInfo() {

}

plugXMLHandler::packageInfo::packageInfo(QString name, QStringList files, QString type, QString description, QString author, QString licence) {

}


plugXMLHandler::plugXMLHandler() {

}

plugXMLHandler::~plugXMLHandler() {

}

QDomDocument plugXMLHandler::createDomFromPackage(packageInfo package_info, int id) {
	QDomDocument doc;

	QDomElement package = doc.createElement("package"); 
	package.attribute("id",QString::number(id)); //надеюсь NULL она просто не добавит
	doc.appendChild(package);
	
	QDomElement package_element;
 	foreach (QString key, package_info.properties) {
 		package_element = doc.createElement(key);
 		package_element.appendChild(doc.createTextNode(package_info.properties[key]));
 		package.appendChild(package_element);
 	}

	package_element = doc.createElement("files");
	package.appendChild(package_element);
	foreach (QString name, package_info.files) {
		QDomElement file_name = doc.createElement("name");
		file_name.appendChild(doc.createTextNode(name));
		package_element.appendChild(file_name);		
	}
	return doc;
}

plugXMLHandler::packageInfo plugXMLHandler::createPackageInfoFromDom(QDomDocument& doc, QString id) {
// 	if (!QString::isEmpty(id))
// 		QDomNode n = doc.elementById();
// 	else
// 		doc.firstChild();
	return packageInfo();
}

plugXMLHandler::packageInfo plugXMLHandler::createPackageInfoFromDom(QDomDocument& doc) {
	
	QDomNode n = doc.documentElement().firstChild();
	qDebug () << "start parser";
	packageInfo package_info;
	while (!n.isNull()) {
	    QDomElement e = n.toElement(); // try to convert the node to an element.
	    if (!e.isNull()) {
			qDebug() << e.tagName() << " : " << e.text();
	        package_info.properties[e.tagName()] =e.text();
	    }
	    n = n.nextSibling();
	}
	return package_info;
}


bool plugXMLHandler::registerPackage(packageInfo package_info) {
	return true;
}

QStringList plugXMLHandler::removePackage(int package_id) {
	return QStringList();
}


plugXMLHandler::packageInfo plugXMLHandler::getPackageInfo(const QString& filename) {
	QDomDocument doc;
	
	QFile input(filename);
	if (!input.open(QIODevice::ReadOnly)) {
		//x3
	}
	if (!doc.setContent(&input)) {
		// x3
	}
	
	return createPackageInfoFromDom(doc);
}

plugXMLHandler::packageInfo plugXMLHandler::getPackageInfo(const QUrl& url) {
	return packageInfo();
}

bool plugXMLHandler::isValid(QDomDocument doc) {
	return true;
}

