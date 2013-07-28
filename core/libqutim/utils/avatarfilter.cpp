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

#include "avatarfilter.h"
#include <QPainter>
#include <QIcon>
#include <QStringBuilder>
#include <QPixmapCache>
#include <QDebug>
#include "avatariconengine_p.h"

namespace qutim_sdk_0_3
{

class AvatarFilterPrivate
{
public:
	QSize defaultSize;
	Qt::AspectRatioMode mode;
	int radius;
};

AvatarFilter::AvatarFilter(const QSize& defaultSize/*, Qt::AspectRatioMode mode*/) :
	d_ptr(new AvatarFilterPrivate)
{
	Q_D(AvatarFilter);
	d->defaultSize = defaultSize;
	d->mode = Qt::IgnoreAspectRatio;	
	d->radius = 5;
#if defined(QUTIM_MOBILE_UI) && !defined(Q_WS_MAEMO_5)
	d->radius = 15;
#endif
}

AvatarFilter::~AvatarFilter()
{

}

bool AvatarFilter::draw(QPainter *painter, int x, int y,
						const QString &path, const QIcon &overlayIcon) const
{
	Q_D(const AvatarFilter);
	if (path.isEmpty())
		return false;

	QString key = QLatin1Literal("qutim_avatar_")
			% QString::number(d->defaultSize.width())
			% QLatin1Char('_')
			% QString::number(d->defaultSize.height())
			% QLatin1Char('_')
			% path;
	QPixmap pixmap;
	if (!QPixmapCache::find(key, &pixmap)) {
		if (!pixmap.load(path))
			return false;
		QString alphaKey = QLatin1Literal("qutim_avatar_alpha_")
				% QString::number(d->defaultSize.width())
				% QLatin1Char('_')
				% QString::number(d->defaultSize.height());
		QPixmap alpha;
		if (!QPixmapCache::find(alphaKey, &alpha)) {
			alpha = QPixmap(d->defaultSize);
			alpha.fill(QColor(0,0,0));
			QPainter painter(&alpha);
			QPen pen(QColor(127,127,127));
			painter.setRenderHint(QPainter::Antialiasing);
			pen.setWidth(0);
			painter.setPen(pen);
			painter.setBrush(QBrush(QColor(255,255,255)));	
			painter.drawRoundedRect(QRectF(QPointF(0, 0),
										   QSize(d->defaultSize.width() - 1,
												 d->defaultSize.height() -1 )),
									d->radius, d->radius);
			painter.end();
			QPixmapCache::insert(alphaKey, alpha);
		}
		int cropSize = qMin(pixmap.width(), pixmap.height());
		pixmap = pixmap.copy(0, 0, cropSize, cropSize);
		if (cropSize > d->defaultSize.width() * 2)
			pixmap = pixmap.scaled(d->defaultSize * 2, d->mode, Qt::FastTransformation);
		pixmap = pixmap.scaled(d->defaultSize, d->mode, Qt::SmoothTransformation);
        {
//            pixmap.setAlphaChannel(alpha);
            QImage image = pixmap.toImage();
            image.setAlphaChannel(alpha.toImage());
            pixmap = QPixmap::fromImage(image);
        }
		QPixmapCache::insert(key, pixmap);
	}
	painter->drawPixmap(x, y, pixmap.width(), pixmap.height(), pixmap);
	QSize overlaySize = d->defaultSize/(d->defaultSize.width() <= 16 ? 1.3 : 2);
	QPixmap overlayPixmap = overlayIcon.pixmap(overlaySize);
	overlaySize = overlayPixmap.size();
	painter->drawPixmap(x + d->defaultSize.width() - overlaySize.width(),
						y + d->defaultSize.height() - overlaySize.height(),
						overlayPixmap
						);
	return true;
}

QIcon AvatarFilter::icon(const QString &path, const QIcon &overlayIcon)
{
	if(path.isEmpty())
		return overlayIcon;
	return QIcon(new AvatarIconEngine(path,overlayIcon));
}

}
