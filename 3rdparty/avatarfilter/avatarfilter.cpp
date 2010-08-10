#include "avatarfilter.h"
#include <QPainter>
#include <QIcon>

namespace qutim_sdk_0_3
{
	struct AvatarFilterPrivate
	{
		QSize defaultSize;
		Qt::AspectRatioMode mode;
	};
	
	AvatarFilter::AvatarFilter(const QSize& defaultSize/*, Qt::AspectRatioMode mode*/) : p(new AvatarFilterPrivate)
	{
		p->defaultSize =defaultSize;
		p->mode = Qt::IgnoreAspectRatio;
	}
	
	AvatarFilter::~AvatarFilter()
	{

	}

	QPixmap AvatarFilter::draw(const QString& path, const QIcon& overlayIcon)
	{
		QSize overlaySize = p->defaultSize / 2; //FIXME
		QIcon icon (path);
		return draw(icon.pixmap(icon.actualSize(QSize(65535,65535))),overlayIcon.pixmap(overlaySize));
	}
	
	QPixmap AvatarFilter::draw(const QPixmap& source, const QPixmap& overlay)
	{
		QPixmap result;
		if(!source.isNull())
		{
			//lets crop pixmap
			int cropSize = qMin(source.width(),source.height());
			result = source.copy(QRect(0,0,cropSize,cropSize));
			result = result.scaled(p->defaultSize,p->mode,Qt::SmoothTransformation);
			static QPixmap alpha;
			if( alpha.size() != p->defaultSize )
			{
				alpha = QPixmap(result.size());
				alpha.fill(QColor(0,0,0));
				QPainter painter(&alpha);
				QPen pen(QColor(127,127,127));
				painter.setRenderHint(QPainter::Antialiasing);
				pen.setWidth(0);
				painter.setPen(pen);
				painter.setBrush(QBrush(QColor(255,255,255)));
				painter.drawRoundedRect(QRectF(QPointF(0,0),QSize(p->defaultSize.width()-1,p->defaultSize.height()-1)),5,5);
				painter.end();
			}
			result.setAlphaChannel(alpha);

			QSize size = overlay.size();
			QPainter painter(&result);
			painter.drawPixmap(	result.width()-size.width(),
								result.height()-size.height(),
								size.width(),
								size.height(),
								overlay);

		}
		return result;
	}


}
