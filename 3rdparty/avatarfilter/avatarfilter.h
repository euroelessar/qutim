#ifndef AVATARFILTER_H
#define AVATARFILTER_H
#include <QString>
#include <QScopedPointer>
#include <QGraphicsEffect>
#include "libqutim/libqutim_global.h"

class QIcon;
namespace qutim_sdk_0_3
{

class Status;
class AvatarFilterPrivate;
class LIBQUTIM_EXPORT AvatarFilter
{
	Q_DECLARE_PRIVATE(AvatarFilter)
public:
	AvatarFilter(const QSize &defaultSize/*, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio*/);
	~AvatarFilter();
	bool draw(QPainter *painter, int x, int y,
			  const QString &path,const QIcon &overlayIcon) const;
	static QIcon icon(const QString &path,const QIcon &overlayIcon);
private:
	QScopedPointer<AvatarFilterPrivate> d_ptr;
};

}
#endif // AVATARFILTER_H
