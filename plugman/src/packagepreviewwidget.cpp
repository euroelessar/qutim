/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "packagepreviewwidget.h"
#include <QPainter>

// From KNewStuff
enum {
	PreviewWidth  = 96,
	PreviewHeight = 72
};

PackagePreviewWidget::PackagePreviewWidget(QWidget *parent)
    : QWidget(parent)
{
}

QSize PackagePreviewWidget::minimumSizeHint() const
{
	return QSize(PreviewWidth, PreviewHeight);
}

QSize PackagePreviewWidget::sizeHint() const
{
	return QSize(PreviewWidth, PreviewHeight);
}

void PackagePreviewWidget::setPreview(const QPixmap &preview)
{
	if (m_preview.cacheKey() == preview.cacheKey())
		return;
	m_preview = preview;
	update();
}

void PackagePreviewWidget::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	p.drawPixmap(0, 0, m_preview);
}
