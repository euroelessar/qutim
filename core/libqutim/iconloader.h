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

#ifndef ICONLOADER_H
#define ICONLOADER_H

#include "libqutim_global.h"

class QIcon;
class QMovie;

namespace qutim_sdk_0_3
{
class IconWrapperPrivate;
class LIBQUTIM_EXPORT IconWrapper
{
public:
	enum StdSize
	{
		StdSize16  = 16,
		StdSize22  = 22,
		StdSize32  = 32,
		StdSize28  = 28,
		StdSize64  = 64,
		StdSize128 = 128
	};

	virtual ~IconWrapper();

	virtual QIcon doLoadIcon(const QString &name) = 0;
	virtual QMovie *doLoadMovie(const QString &name) = 0;
	virtual QString doIconPath(const QString &name, uint iconSize) = 0;
	virtual QString doMoviePath(const QString &name, uint iconSize) = 0;
};
}

Q_DECLARE_INTERFACE(qutim_sdk_0_3::IconWrapper, "org.qutim.IconWrapper/1.1")

namespace qutim_sdk_0_3
{
class LIBQUTIM_EXPORT IconLoader : public QObject, protected qutim_sdk_0_3::IconWrapper
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::IconWrapper)
	Q_CLASSINFO("Service", "IconLoader")
public:
	enum StdSize
	{
		StdSize16  = 16,
		StdSize22  = 22,
		StdSize32  = 32,
		StdSize28  = 28,
		StdSize64  = 64,
		StdSize128 = 128
	};
	IconLoader();
	virtual ~IconLoader();

	static QIcon loadIcon(const QString &name);
	static QMovie *loadMovie(const QString &name);
	static QString iconPath(const QString &name, uint iconSize);
	static QString moviePath(const QString &name, uint iconSize);

protected:
	void virtual_hook(int id, void *data);
	friend class IconWrapperPrivate;
};
}

#endif // ICONLOADER_H

