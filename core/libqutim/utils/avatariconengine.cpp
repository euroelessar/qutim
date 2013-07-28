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

#include "avatariconengine_p.h"
#include "avatarfilter.h"
#include <QPainter>

namespace qutim_sdk_0_3
{

AvatarIconEngine::AvatarIconEngine(const QString &path,const QIcon &overlay) :
	m_path(path),
	m_overlay(overlay)
{

}

AvatarIconEngine::~AvatarIconEngine()
{

}

void AvatarIconEngine::paint(QPainter *painter, const QRect &rect,
							 QIcon::Mode mode, QIcon::State state)
{
	Q_UNUSED(mode);
	Q_UNUSED(state);
	painter->drawPixmap(rect, pixmap(rect.size(), mode, state));
}

QSize AvatarIconEngine::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
	QPixmap img(m_path);
	if(img.isNull())
		return m_overlay.actualSize(size,mode,state);
	if(img.size().width() < size.width() || img.size().height() < size.height())
		return img.size();
	return size;
}

QString AvatarIconEngine::key() const
{
    return QLatin1String("AvatarIconEngiixne");
}

AvatarIconEngine *AvatarIconEngine::clone() const
{
    return new AvatarIconEngine(*this);
}

QPixmap AvatarIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
	QPixmap pixmap(size);
	pixmap.fill(Qt::transparent);
	QPainter p;
	p.begin(&pixmap);
	bool hasAvatar = AvatarFilter(size).draw(&p,0,0,m_path,m_overlay);
	p.end();

	if(!hasAvatar)
		return m_overlay.pixmap(size,mode,state);
	else
		return pixmap;
}

} //qutim_sdk_0_3
