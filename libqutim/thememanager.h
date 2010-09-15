#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{

	class LIBQUTIM_EXPORT ThemeManager
	{
	public:
		static QString path(const QString &category, const QString &themeName);
		static QStringList list(const QString &category);
	private:
		ThemeManager();
		~ThemeManager();
		Q_DISABLE_COPY(ThemeManager);
	};

}

#endif // THEMEMANAGER_H
