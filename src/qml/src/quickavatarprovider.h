#ifndef QUTIM_SDK_0_3_QUICKAVATARPROVIDER_H
#define QUTIM_SDK_0_3_QUICKAVATARPROVIDER_H

#include <QQuickImageProvider>

namespace qutim_sdk_0_3 {

class QuickAvatarProvider : public QQuickImageProvider
{
public:
	QuickAvatarProvider();

	QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKAVATARPROVIDER_H
