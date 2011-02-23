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
	QDir dir(QLatin1String(":/icons"));
	m_icons = dir.entryList(QDir::Files, QDir::Name);
	for (int i = 0; i < m_icons.size(); i++)
		m_icons[i].chop(4);
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

struct ResourceIconNameComparator
{
	// Return true if a is less then b
	bool operator()(const QString &a, const QStringRef &b)
	{
		return QStringRef(&a) < b;
	}
	bool operator()(const QStringRef &a, const QString &b)
	{
		return a < QStringRef(&b);
	}

};

QString ResourceIconLoader::iconPath(const QString &name, uint iconSize)
{
	Q_UNUSED(iconSize);
	int index = name.length();
	ResourceIconNameComparator comp;
	QStringList::const_iterator it;
	while (index != -1) {
		it = qBinaryFind(m_icons.constBegin(), m_icons.constEnd(), QStringRef(&name, 0, index), comp);
		if (it != m_icons.constEnd())
			return nameInResource(*it);
		index = name.lastIndexOf('-', index - 1);
	}
	return QString();
}

QString ResourceIconLoader::moviePath(const QString &, uint)
{
	//IMPLEMENT ME
	return QString();
}

}
