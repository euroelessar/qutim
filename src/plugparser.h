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

#ifndef PLUGPARSER_H
#define PLUGPARSER_H
#include <QFile>
#include <QtXml>
#include <QUrl>

class plugParser : public QObject
{
Q_OBJECT
public:
	plugParser (QObject *parent = 0);
        ~plugParser ();
	QHash<QString, QString> parseItem (const QString &filename);
	QHash<QString, QString> parseItem (const QUrl &url);
private:
	bool parser ();
	QDomElement *docElem;
	QDomDocument doc;
	QHash<QString, QString> packItem;
	QString lastError;
};

#endif // PLUGPARSER_H
