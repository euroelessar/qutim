/****************************************************************************
 *  avatariconengine.cpp
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
