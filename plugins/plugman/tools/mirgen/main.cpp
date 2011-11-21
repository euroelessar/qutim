/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include <QtCore/QCoreApplication>
#include <QDomDocument>
#include <QZipReader/qzipreader.h>
#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
	QStringList args = a.arguments();
	QDomDocument doc;
	QDomElement packages = doc.createElement("packages");
	foreach(const QString &arg, args)
	{
		QDir dir(arg);
		if(!dir.exists())
			continue;
		QFileInfoList files = dir.entryInfoList(QStringList() << "*.zip");
		foreach(const QFileInfo &file, files)
		{
			QZipReader reader(file.absoluteFilePath());
			QByteArray data = reader.fileData("Pinfo.xml");
			QDomDocument package;
			if(data.isEmpty() || !package.setContent(data, false))
				continue;
			QDomElement element = package.documentElement();
			if(element.hasAttribute("id"))
				element.removeAttribute("id");
			if(element.toElement().elementsByTagName("category").isEmpty())
			{
				QDomElement node = package.createElement("category");
				node.appendChild(package.createTextNode("art"));
				element.appendChild(node);
			}
			if(element.toElement().elementsByTagName("platform").isEmpty())
			{
				QDomElement node = package.createElement("platform");
				node.appendChild(package.createTextNode("all"));
				element.appendChild(node);
			}
			packages.appendChild(element);
		}
	}
	doc.appendChild(packages);
	qDebug("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n%s", qPrintable(doc.toString(4)));
	return 0;
}

