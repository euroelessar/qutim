#ifndef QUICKCHAT_TRIANGLEIMAGEPROVIDER_H
#define QUICKCHAT_TRIANGLEIMAGEPROVIDER_H

#include <QQuickImageProvider>

namespace QuickChat {

class TriangleImageProvider : public QQuickImageProvider
{
public:
	TriangleImageProvider();

	QImage requestImage(const QString &id, QSize *size, const QSize& requestedSize) override;
};

} // namespace QuickChat

#endif // QUICKCHAT_TRIANGLEIMAGEPROVIDER_H
