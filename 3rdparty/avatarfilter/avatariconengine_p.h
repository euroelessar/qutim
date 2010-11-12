#ifndef AVATARICONENGINE_P_H
#define AVATARICONENGINE_P_H

#include <QtGui/QIconEngineV2>

namespace qutim_sdk_0_3
{

/**
  @private
*/
class AvatarIconEngine : public QIconEngineV2
{
public:
	AvatarIconEngine(const QString &path,const QIcon &overlay);
	virtual ~AvatarIconEngine();

	virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);
	virtual QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state);
	virtual QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);

	virtual QString key() const;
private:
	QString m_path;
	QIcon m_overlay;
};

} //qutim_sdk_0_3
#endif // AVATARICONENGINE_P_H
