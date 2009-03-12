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
#include <QHash>
/*
plugXMLHandler::packageInfo::packageInfo(QString m_name, QStringList m_files, QString m_type, QString m_description, QString m_author, QString m_licence) {
	name = m_name;
	description = m_description;
	licence = m_licence;
	author = m_author;
	files = m_files;
}
*/

plugXMLHandler::plugXMLHandler() {

}

plugXMLHandler::~plugXMLHandler() {

}

QDomDocument plugXMLHandler::createDomFromPackage(plugXMLHandler::packageInfo package_info, int id) {
	QDomDocument doc;
	
	QHash<QString,QDomElement> package_elements;
	//FIXME охота это сделать как нить одним махом красиво через итераторы, но пока побуду китайцем (=
	//но ваще это некрасиво и громозко, но зато в отличии от итераторов позволяет добавлять строго те теги, какие надо.
	package_elements["package"] = doc.createElement("package");
	package_elements["name"] = doc.createElement("name");
	package_elements["type"] = doc.createElement("type");
	package_elements["description"] = doc.createElement("description");
	package_elements["files"] = doc.createElement("files");
	package_elements["author"] = doc.createElement("author");
	package_elements["licence"] = doc.createElement("licence");
	
	package_elements["package"].attribute("id",QString::number(id)); //надеюсь NULL она просто не добавит
	doc.appendChild(package_elements["package"]);
	
	package_elements["package"].appendChild(package_elements["name"]);
	package_elements["package"].appendChild(package_elements["type"]);
	package_elements["package"].appendChild(package_elements["description"]);
	package_elements["package"].appendChild(package_elements["author"]);
	package_elements["package"].appendChild(package_elements["licence"]);
	
	package_elements["name"].appendChild(doc.createTextNode(package_info.name));
	package_elements["type"].appendChild(doc.createTextNode(package_info.type));
	package_elements["description"].appendChild(doc.createTextNode(package_info.description));
	package_elements["author"].appendChild(doc.createTextNode(package_info.author));
	package_elements["licence"].appendChild(doc.createTextNode(package_info.licence));

	//а это уже по уму сделано (=
	foreach (QString name, package_info.files) {
		QDomElement file_name = doc.createElement("name");
		file_name.appendChild(doc.createTextNode(name));
		package_elements["files"].appendChild(file_name);		
	}
	return doc;
}

plugXMLHandler::packageInfo plugXMLHandler::createPackageInfoFromDom(QDomDocument& doc, int id) {

}

bool plugXMLHandler::registerPackage(plugXMLHandler::packageInfo package_info) {

}

QStringList plugXMLHandler::removePackage(int package_id) {

}


plugXMLHandler::packageInfo plugXMLHandler::getPackageInfo(const QString& filename) {
	QDomDocument doc;
	
	if (!doc.setContent(filename)) {
		// x3
	}
	
	return createPackageInfoFromDom(doc);
}

plugXMLHandler::packageInfo plugXMLHandler::getPackageInfo(const QUrl& url) {

}

bool plugXMLHandler::isValid(QDomDocument doc) {
	return true;
}

