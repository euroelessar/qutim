#include "qresourceiconloader.h"
#include <QIcon>
#include <QDir>
#include <QLatin1Literal>
#include "modulemanagerimpl.h"
#include <libqutim/debug.h>
#include <QFile>

namespace Core
{

QString nameInResource (const QString &name)
{
	//FIXME,
	return QLatin1Literal(":/icons/") % name % QLatin1Literal(".png");
}

ResourceIconLoader::ResourceIconLoader()
{
	//TODO implement custom resource loading logic
}

QIcon ResourceIconLoader::loadIcon(const QString &originName)
{
	QIcon icon;
	QString name = nameInResource(originName);
	if (QFile::exists(name)) {
		icon = QIcon(name);
		debug() << "Loaded icon" << name;
		return icon;
	}
	//fallbacks, TODO add cache support
	QDir dir(":/icons");
	QStringList entries = dir.entryList(QDir::Files,QDir::Name | QDir::Reversed);

	for (int i = 0; i != entries.count(); i++) {
		name = entries.at(i).section('/', -1, -1);
		int index = name.lastIndexOf(QLatin1Char('.'));
		if (index < 0)
			continue;
		name.truncate(index);
		if ((name.size() < originName.size()
				&& originName.startsWith(name)
				&& originName.at(name.size()) == '-')) {
			icon = QIcon(nameInResource(name));
			break;
		}
	}

	debug() << "Loaded fallback icon" << name << originName;

	return icon;
}

QMovie *ResourceIconLoader::loadMovie(const QString &)
{
	//IMPLEMENT ME
	return 0;
}

QString ResourceIconLoader::iconPath(const QString &name, uint iconSize)
{
	return QString();
}

QString ResourceIconLoader::moviePath(const QString &, uint)
{
	//IMPLEMENT ME
	return QString();
}

}
