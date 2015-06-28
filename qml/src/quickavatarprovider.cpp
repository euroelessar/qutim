#include "quickavatarprovider.h"
#include <QDebug>
#include <qutim/icon.h>
#include <QUrlQuery>
#include <qutim/avatarfilter.h>

namespace qutim_sdk_0_3 {

QuickAvatarProvider::QuickAvatarProvider()
	: QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

/*!
 * Image {
 *     source: "image://avatar/path?name=icon-name"
 * }
 *
 * Image {
 *     source: "image://avatar/path?size=64&name=icon-name"
 * }
 */
QPixmap QuickAvatarProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
	const QUrl url(QStringLiteral("file:///") + id);
	const QUrlQuery query(url.query());
	const QString filePath = url.path();
	const QString iconName = query.queryItemValue(QStringLiteral("name"));
	const QString iconSize = query.queryItemValue(QStringLiteral("size"));

	QSize tmp;
	if (!size)
		size = &tmp;

	bool ok = false;
	int iconSizeValue = iconSize.toInt(&ok);
	if (ok)
		*size = QSize(iconSizeValue, iconSizeValue);
	else if (requestedSize.width() > 0)
		*size = requestedSize;
	else
		*size = QSize(128, 128);

	QIcon icon = Icon(iconName);

	if (filePath.size() > 1)
		icon = AvatarFilter::icon(filePath, icon);

	return icon.pixmap(*size);
}

} // namespace qutim_sdk_0_3
