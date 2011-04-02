#include "qresourceiconloader.h"
#include <QIcon>
#include <QDir>
#include <QLatin1Literal>
#include "modulemanagerimpl.h"
#include <qutim/debug.h>
#include <qutim/systeminfo.h>
#include <QFile>

namespace Embedded
{

ResourceIconLoader::ResourceIconLoader()
{
	//TODO implement custom resource loading logic
	QDir dir(QLatin1String(":/icons"));
	QFileInfoList files = dir.entryInfoList(QDir::Files);
	for (int i = 0; i < files.size(); i++) {
		QString name = files.at(i).baseName();
		QStringRef iconName(&m_buffer, m_buffer.size(), name.size());
		m_buffer.append(name);
		m_icons.insert(iconName, files.at(i).absoluteFilePath());
	}
}

QIcon ResourceIconLoader::loadIcon(const QString &originName)
{
	// TODO: implement IconEngine, look XdgIconEngine for details
	return QIcon(iconPath(originName,0));
}

QMovie *ResourceIconLoader::loadMovie(const QString &)
{
	//IMPLEMENT ME
	return 0;
}

QString ResourceIconLoader::iconPath(const QString &name, uint iconSize)
{
	Q_UNUSED(iconSize);
	int index = name.length();
	IconHash::const_iterator it;
	while (index != -1) {
		it = m_icons.constFind(QStringRef(&name, 0, index));
		if (it != m_icons.constEnd())
			return it.value();
		index = name.lastIndexOf('-', index - 1);
	}
	return QString();
}

QString ResourceIconLoader::moviePath(const QString &, uint)
{
	//IMPLEMENT ME
	return QString();
}

} //namespace Embedded
