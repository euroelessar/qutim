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

#include "plugparser.h"
#include <QDomDocument>

plugParser::plugParser(QObject *parent) {

}

plugParser::~plugParser() {
	delete(docElem);
}

QHash<QString, QString> plugParser::parseItem(const QString& filename) {

	QFile input(filename);

	if (!input.open(QIODevice::ReadOnly))
		lastError = tr("Unable to parse item");
	
	if (!doc.setContent(&input)) {
		lastError = tr("Unable to parse item");
	}
	// print out the element names of all elements that are direct children
	// of the outermost element.
	docElem = &doc.documentElement();
	if (parser())
		return packItem;


}

QHash<QString, QString> plugParser::parseItem(const QUrl& url) {
	lastError = "Ну не сделано ещё";
}


bool plugParser::parser() {

	QDomNode n = docElem->firstChild();
	while(!n.isNull()) {
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if(!e.isNull()) {
			packItem[e.tagName()] =e.text();
		}
		n = n.nextSibling();
	}	
	return true;
}

