#include "kdeiconloader.h"
#include <kiconloader.h>
#include <kicon.h>
#include <kcomponentdata.h>

KdeIconLoader::KdeIconLoader()
{
	KIconLoader::global()->reconfigure("qutim", 0);
}

QIcon KdeIconLoader::doLoadIcon(const QString &name)
{
	return KIcon(name, KIconLoader::global());
}

QMovie *KdeIconLoader::doLoadMovie(const QString &name)
{
	return KIconLoader::global()->loadMovie(name, KIconLoader::NoGroup);
}

QString KdeIconLoader::doIconPath(const QString &name, uint iconSize)
{
	return KIconLoader::global()->iconPath(name, iconSize, true);
}

QString KdeIconLoader::doMoviePath(const QString &name, uint iconSize)
{
	return KIconLoader::global()->moviePath(name, KIconLoader::NoGroup, iconSize);
}
