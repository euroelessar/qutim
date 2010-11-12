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
	};

	AvatarFilter::AvatarFilter(const QSize& defaultSize/*, Qt::AspectRatioMode mode*/) :
			d_ptr(new AvatarFilterPrivate)
	{
		Q_D(AvatarFilter);
		d->defaultSize = defaultSize;
		d->mode = Qt::IgnoreAspectRatio;
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
										5, 5);
				painter.end();
				QPixmapCache::insert(alphaKey, alpha);
			}
			int cropSize = qMin(pixmap.width(), pixmap.height());
			pixmap = pixmap.copy(0, 0, cropSize, cropSize);
			pixmap = pixmap.scaled(d->defaultSize, d->mode, Qt::SmoothTransformation);
			pixmap.setAlphaChannel(alpha);
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
	return QIcon(new AvatarIconEngine(path,overlayIcon));
}

}
