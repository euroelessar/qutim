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
#include <QtGui/QApplication>
#include "generatorwindow.h"
#include "pluglibinfo.h"
#include "plugxmlhandler.h"
#include <QZipReader/qzipwriter.h>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GeneratorWindow w;
	w.show();
//	packageInfo info = plugLibInfo("/home/elessar/qutim/plugins/libyandexnarod.so").toInfo();
//	info.properties["version"] = "0.1.0";
//	QDir dir("/home/elessar/qutim");
//	QZipWriter writer("test.zip");
//	{
//		QByteArray data = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
//		data.append(info.toString().toUtf8());
//		writer.addFile("Pinfo.xml", data);
//	}
//	foreach(const QString filepath, info.files)
//	{
//		QFile file(dir.filePath(filepath));
//		if(file.open(QIODevice::ReadOnly))
//			writer.addFile(filepath, &file);
//	}
//	writer.close();
//	qDebug() << info.toString();

    return a.exec();
}

