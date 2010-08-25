#include "kdeiconloader.h"
#include <kiconloader.h>
#include <kicon.h>

KdeIconLoader::KdeIconLoader()
{
//	KIconLoader::global()->addAppDir("qutim");
}

QIcon KdeIconLoader::loadIcon(const QString &name)
{
	return KIcon(name);
}

QMovie *KdeIconLoader::loadMovie(const QString &name)
{
	return KIconLoader::global()->loadMovie(name, KIconLoader::NoGroup);
}

QString KdeIconLoader::iconPath(const QString &name, uint iconSize)
{
	return KIconLoader::global()->iconPath(name, iconSize, true);
}

QString KdeIconLoader::moviePath(const QString &name, uint iconSize)
{
	return KIconLoader::global()->moviePath(name, KIconLoader::NoGroup, iconSize);
}
