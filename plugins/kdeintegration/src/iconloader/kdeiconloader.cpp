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
#include "kdeiconloader.h"
#include <kiconloader.h>
#include <kicon.h>
#include <kcomponentdata.h>
#include <qutim/debug.h>

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
	return KIconLoader::global()->iconPath(name, -iconSize, true);
}

QString KdeIconLoader::doMoviePath(const QString &name, uint iconSize)
{
	return KIconLoader::global()->moviePath(name, KIconLoader::NoGroup, iconSize);
}

