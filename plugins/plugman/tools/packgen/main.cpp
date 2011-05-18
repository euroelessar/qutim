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
