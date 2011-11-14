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

