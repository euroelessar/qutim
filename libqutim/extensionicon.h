#ifndef EXTENSIONICON_H
#define EXTENSIONICON_H

#include "libqutim_global.h"
#include <QtGui/QIcon>
#include <QtCore/QSharedData>

namespace qutim_sdk_0_3
{
	class ExtensionIconData;

	class LIBQUTIM_EXPORT ExtensionIcon
	{
	public:
		ExtensionIcon(const QString &name = QString());
		ExtensionIcon(const QIcon &icon);
		ExtensionIcon(const ExtensionIcon &other);
		~ExtensionIcon();
		ExtensionIcon &operator =(const ExtensionIcon &other);
		operator QIcon() const { return toIcon(); }
		QIcon toIcon() const;
	private:
		QSharedDataPointer<ExtensionIconData> p;
	};
}

#endif // EXTENSIONICON_H
