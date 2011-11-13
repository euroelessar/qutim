/****************************************************************************
 *  avatarfilter.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *	Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef AVATARFILTER_H
#define AVATARFILTER_H
#include <QString>
#include <QScopedPointer>
#include <QGraphicsEffect>
#include <QIcon>
#include "../../libqutim/libqutim_global.h"


namespace qutim_sdk_0_3
{

class Status;
class AvatarFilterPrivate;
class LIBQUTIM_EXPORT AvatarFilter
{
	Q_DECLARE_PRIVATE(AvatarFilter)
public:
	AvatarFilter(const QSize &defaultSize);
	~AvatarFilter();
	bool draw(QPainter *painter, int x, int y, const QString &path,const QIcon &overlayIcon) const;
	static QIcon icon(const QString &path,const QIcon &overlayIcon = QIcon());
private:
	QScopedPointer<AvatarFilterPrivate> d_ptr;
};

}
#endif // AVATARFILTER_H
