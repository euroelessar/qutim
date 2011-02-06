/****************************************************************************
 *  iconloader.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef ICONLOADER_H
#define ICONLOADER_H

#include "libqutim_global.h"

class QIcon;
class QMovie;

namespace qutim_sdk_0_3
{
class LIBQUTIM_EXPORT IconLoader : public QObject
{
	Q_OBJECT
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
	static IconLoader *instance();
	virtual QIcon loadIcon(const QString &name) = 0;
	virtual QMovie *loadMovie(const QString &name) = 0;
	virtual QString iconPath(const QString &name, uint iconSize) = 0;
	virtual QString moviePath(const QString &name, uint iconSize) = 0;
protected:
	virtual void virtual_hook(int id, void *data);
};
}

#endif // ICONLOADER_H
