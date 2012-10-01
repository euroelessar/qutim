/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef AVATARFILTER_H
#define AVATARFILTER_H
#include <QString>
#include <QScopedPointer>
#include <QGraphicsEffect>
#include <QIcon>
#include "libqutim_global.h"


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
