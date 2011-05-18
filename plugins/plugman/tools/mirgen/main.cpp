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
