/****************************************************************************
 *  iconloader.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
	public:
		enum StdSize
		{
			StdSize16,
			StdSize22,
			StdSize32,
			StdSize28,
			StdSize64,
			StdSize128
		};
		IconLoader();
		virtual ~IconLoader();
		static IconLoader *instance();
		virtual QIcon loadIcon(const QString &name) = 0;
		virtual QMovie *loadMovie(const QString &name) = 0;
		virtual QString iconPath(const QString &name, StdSize iconSize) = 0;
		virtual QString moviePath(const QString &name, StdSize iconSize) = 0;
	};
}

#endif // ICONLOADER_H
