#ifndef AVATARFILTER_H
#define AVATARFILTER_H
#include <QString>
#include <QScopedPointer>
#include <QGraphicsEffect>

class QIcon;
namespace qutim_sdk_0_3
{
	class Status;
	struct AvatarFilterPrivate;
	class AvatarFilter
	{
	public:
		AvatarFilter(const QSize &defaultSize, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio);
		QPixmap draw(const QString &path,const QIcon &overlayIcon);
		QPixmap draw(const QPixmap &source, const QPixmap &overlay);
		~AvatarFilter();
	private:
		QScopedPointer<AvatarFilterPrivate> p;
	};

}
#endif // AVATARFILTER_H
